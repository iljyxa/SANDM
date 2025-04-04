#include <utility>

#include "../include/virtual_machine_controller.hpp"

VirtualMachineController::VirtualMachineController(ProcessorIo* processor_io, QObject* parent) :
    QObject(parent),
    state_(STOPPED),
    debugging_(false) {
    SetProcessorIo(processor_io);
}

// === Управление машиной ===

void VirtualMachineController::Load(const common::ByteCode& byte_code,
                                    common::SourceToBytecodeMap& source_to_bytecode_map) {
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
    SetProcessorObserver(nullptr);

    if (state_ == STOPPED) {
        // Так как машина остановлена, необходимо встать на первую инструкцию, но не выполнять ее
        SetState(PAUSED);
    } else {
        SetState(RUNNING);
        VirtualMachine::Step();
        SetState(PAUSED);
    }
}

void VirtualMachineController::OnReset() {
    SetProcessorObserver(nullptr);
    VirtualMachine::Reset();
    emit Update();
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
    if (const common::DoubleByte ip = processor_->GetInstructionPointer(); bytecode_to_source_map_.contains(ip)) {
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

void VirtualMachineController::OnPageTableIndexEdited(const int value) {
    SetPageTableIndex(value);
    emit Update();
}

// === Методы-наблюдатели, реализующие интерфейс ProcessorObserver ===

void VirtualMachineController::OnRegisterIpChanged(common::DoubleByte& instruction_pointer) {
    if (debugging_ && breakpoints_.contains(instruction_pointer)) {
        VirtualMachine::Stop();
        SetState(PAUSED);
    }
}

void VirtualMachineController::OnRegisterAccChanged(const common::Bytes& accumulator) {
}

void VirtualMachineController::OnRegisterAuxChanged(const common::Bytes& auxiliary) {
}

void VirtualMachineController::OnRegisterPtiChanged(common::Byte& page_table_index) {
}

void VirtualMachineController::OnMemoryChanged(common::DoubleByte& address) {
}

void VirtualMachineController::OnStatusChanged(bool& is_running) {
}
