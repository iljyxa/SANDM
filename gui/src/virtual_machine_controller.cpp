
#include "gui/virtual_machine_controller.hpp"

VirtualMachineController::VirtualMachineController(ProcessorIo* processor_io, QObject* parent) :
    QObject(parent),
    state_(STOPPED),
    debugging_(false) {
    SetProcessorIo(processor_io);
}

// === Управление машиной ===

void VirtualMachineController::Load(const snm::ByteCode& byte_code,
                                    snm::SourceToBytecodeMap& source_to_bytecode_map) {
    VirtualMachine::Load(byte_code);
    source_to_bytecode_map_ = std::move(source_to_bytecode_map);

    bytecode_to_source_map_.clear();
    for (auto& [source_line, bytecode_line] : source_to_bytecode_map_) {
        bytecode_to_source_map_[bytecode_line] = source_line;
    }

    UpdateBreakpoints();
}

void VirtualMachineController::OnRun() {
    SetProcessorObserver(debugging_ ? this : nullptr);

    if (state_ == STOPPED) {
        memory_manager_->ResetData();
    }

    SetState(RUNNING);

    QThreadPool::globalInstance()->start([this] {
        try {
            VirtualMachine::Run();
        } catch (const std::exception& e) {
            SetState(STOPPED);
            emit ErrorOccurred(QString(e.what()));
            return;
        }
        if (state_ != PAUSED) {
            SetState(STOPPED);
        } else {
            emit Update();
        }
    });
}

void VirtualMachineController::OnStop() {
    SetProcessorObserver(nullptr);

    VirtualMachine::Stop();
    processor_->Reset();
    memory_manager_->ResetData();
    SetState(STOPPED);
}

void VirtualMachineController::OnStep() {
    if (state_ == STOPPED) {
        processor_->Reset();
        memory_manager_->ResetData();

        SetProcessorObserver(this);
        // Так как машина остановлена, необходимо встать на первую инструкцию, но не выполнять ее
        SetState(PAUSED);
    } else {
        SetState(RUNNING);
        VirtualMachine::Step();

        const snm::ProcessorState state = processor_->GetState();
        if (state != snm::ProcessorState::STOPPED) {
            SetState(PAUSED);
        } else {
            SetState(STOPPED);
        }

    }
}

void VirtualMachineController::OnReset() {
    SetProcessorObserver(nullptr);
    VirtualMachine::Reset();
    emit Update();
    emit Reseted();
}

void VirtualMachineController::ResetProcessor() const {
    processor_->Reset();
}

void VirtualMachineController::OnDebug() {
    debugging_ = true;
    OnRun();
}

void VirtualMachineController::OnPauseContinue() {
    if (IsRunning()) {
        VirtualMachine::Stop();
        SetState(PAUSED);
    } else {
        OnRun();
        // Слот не требуется, так как бросается в OnRun
    }
}

void VirtualMachineController::SetState(const VmState state) {
    if (state_ == state) {
        return;
    }

    if (state == STOPPED) {
        debugging_ = false;
    }

    state_ = state;
    emit StateChanged(state_, debugging_);
    emit Update();
}

VmState VirtualMachineController::GetState() const {
    return state_;
}

unsigned int VirtualMachineController::GetCurrentCodeLine() {
    if (const snm::Address ip = processor_->GetInstructionPointer(); bytecode_to_source_map_.contains(ip)) {
        return bytecode_to_source_map_[ip];
    }

    return 0;
}

// === Точки останова ===

void VirtualMachineController::OnInsertBreakpoint(const unsigned int breakpoint) {
    source_breakpoints_.insert(breakpoint);

    if (source_to_bytecode_map_.contains(breakpoint)) {
        breakpoints_.insert(source_to_bytecode_map_[breakpoint]);
    }
}

void VirtualMachineController::OnRemoveBreakpoint(const unsigned int breakpoint) {
    source_breakpoints_.remove(breakpoint);
    if (source_to_bytecode_map_.contains(breakpoint)) {
        breakpoints_.remove(source_to_bytecode_map_[breakpoint]);
    }
}

void VirtualMachineController::OnClearBreakpoints() {
    breakpoints_.clear();
}

void VirtualMachineController::UpdateBreakpoints() {
    breakpoints_.clear();

    for (auto breakpoint : source_breakpoints_) {
        if (source_to_bytecode_map_.contains(breakpoint)) {
            breakpoints_.insert(source_to_bytecode_map_[breakpoint]);
        }
    }
}

// === Обработчики изменений регистров ===

void VirtualMachineController::OnAccumulatorEdited(const int value) {
    SetAccumulator(value);
}

void VirtualMachineController::OnAuxiliaryEdited(const int value) {
    SetAuxiliary(value);
}

void VirtualMachineController::OnInstructionPointerEdited(const int value) {
    SetInstructionPointer(value);
}

// === Методы-наблюдатели, реализующие интерфейс ProcessorObserver ===

void VirtualMachineController::OnRegisterIpChanged(const snm::Address& instruction_pointer) {
    if (debugging_ && breakpoints_.contains(instruction_pointer)) {
        VirtualMachine::Stop();
        SetState(PAUSED);
    }
    emit StateChanged(state_, debugging_);
    emit Update();
}

void VirtualMachineController::OnRegisterAccChanged(const snm::Bytes& accumulator) {
}

void VirtualMachineController::OnRegisterAuxChanged(const snm::Bytes& auxiliary) {
}

void VirtualMachineController::OnMemoryChanged(const snm::Address& address) {
}

void VirtualMachineController::OnStateChanged(const snm::ProcessorState& state) {
}
