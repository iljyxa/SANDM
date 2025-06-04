#include "../include/core/processor.hpp"

Processor::Processor(MemoryManager& memory, ProcessorObserver* observer, ProcessorIo* io) :
    memory_(memory),
    observer_(observer),
    io_(io),
    is_running_(false) {

    argument_modifiers_[static_cast<uint8_t>(common::ArgModifier::NONE)] = common::ArgModifier::NONE;
    argument_modifiers_[static_cast<uint8_t>(common::ArgModifier::REF)] = common::ArgModifier::REF;
    argument_modifiers_[static_cast<uint8_t>(common::ArgModifier::REF_REF)] = common::ArgModifier::REF_REF;

    // nope
    instructions_handlers_[InstructionByte(common::OpCode::NOPE, common::TypeModifier::C)] = [this] {
        Nope();
    };
    instructions_handlers_[InstructionByte(common::OpCode::NOPE, common::TypeModifier::W)] = [this] {
        Nope();
    };
    instructions_handlers_[InstructionByte(common::OpCode::NOPE, common::TypeModifier::SW)] = [this] {
        Nope();
    };
    instructions_handlers_[InstructionByte(common::OpCode::NOPE, common::TypeModifier::R)] = [this] {
        Nope();
    };


    // add
    instructions_handlers_[InstructionByte(common::OpCode::ADD, common::TypeModifier::C)] = [this] {
        Add<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::ADD, common::TypeModifier::W)] = [this] {
        Add<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::ADD, common::TypeModifier::SW)] = [this] {
        Add<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::ADD, common::TypeModifier::R)] = [this] {
        Add<common::Real>();
    };

    // sub
    instructions_handlers_[InstructionByte(common::OpCode::SUB, common::TypeModifier::C)] = [this] {
        Sub<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SUB, common::TypeModifier::W)] = [this] {
        Sub<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SUB, common::TypeModifier::SW)] = [this] {
        Sub<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SUB, common::TypeModifier::R)] = [this] {
        Sub<common::Real>();
    };

    // mul
    instructions_handlers_[InstructionByte(common::OpCode::MUL, common::TypeModifier::C)] = [this] {
        Mul<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::MUL, common::TypeModifier::W)] = [this] {
        Mul<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::MUL, common::TypeModifier::SW)] = [this] {
        Mul<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::MUL, common::TypeModifier::R)] = [this] {
        Mul<common::Real>();
    };

    // div
    instructions_handlers_[InstructionByte(common::OpCode::DIV, common::TypeModifier::C)] = [this] {
        Div<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::DIV, common::TypeModifier::W)] = [this] {
        Div<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::DIV, common::TypeModifier::SW)] = [this] {
        Div<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::DIV, common::TypeModifier::R)] = [this] {
        Div<common::Real>();
    };

    // mod
    instructions_handlers_[InstructionByte(common::OpCode::MOD, common::TypeModifier::C)] = [this] {
        Mod<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::MOD, common::TypeModifier::W)] = [this] {
        Mod<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::MOD, common::TypeModifier::SW)] = [this] {
        Mod<common::SignedWord>();
    };

    // load
    instructions_handlers_[InstructionByte(common::OpCode::LOAD, common::TypeModifier::C)] = [this] {
        Load<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::LOAD, common::TypeModifier::W)] = [this] {
        Load<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::LOAD, common::TypeModifier::SW)] = [this] {
        Load<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::LOAD, common::TypeModifier::R)] = [this] {
        Load<common::Real>();
    };

    // store
    instructions_handlers_[InstructionByte(common::OpCode::STORE, common::TypeModifier::W)] = [this] {
        Store();
    };

    // input
    instructions_handlers_[InstructionByte(common::OpCode::INPUT, common::TypeModifier::C)] = [this] {
        Input<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::INPUT, common::TypeModifier::W)] = [this] {
        Input<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::INPUT, common::TypeModifier::SW)] = [this] {
        Input<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::INPUT, common::TypeModifier::R)] = [this] {
        Input<common::Real>();
    };

    // output
    instructions_handlers_[InstructionByte(common::OpCode::OUTPUT, common::TypeModifier::C)] = [this] {
        Output<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::OUTPUT, common::TypeModifier::W)] = [this] {
        Output<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::OUTPUT, common::TypeModifier::SW)] = [this] {
        Output<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::OUTPUT, common::TypeModifier::R)] = [this] {
        Output<common::Real>();
    };

    // jump
    instructions_handlers_[InstructionByte(common::OpCode::JUMP, common::TypeModifier::W)] = [this] {
        Jump();
    };

    // skiplower
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_LOWER, common::TypeModifier::C)] = [this] {
        SkipLower<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_LOWER, common::TypeModifier::W)] = [this] {
        SkipLower<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_LOWER, common::TypeModifier::SW)] = [this] {
        SkipLower<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_LOWER, common::TypeModifier::R)] = [this] {
        SkipLower<common::Real>();
    };

    // skipgreater
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_GREATER, common::TypeModifier::C)] = [this] {
        SkipGreater<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_GREATER, common::TypeModifier::W)] = [this] {
        SkipGreater<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_GREATER, common::TypeModifier::SW)] = [this] {
        SkipGreater<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_GREATER, common::TypeModifier::R)] = [this] {
        SkipGreater<common::Real>();
    };

    // skipequal
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_EQUAL, common::TypeModifier::C)] = [this] {
        SkipEqual<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_EQUAL, common::TypeModifier::W)] = [this] {
        SkipEqual<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_EQUAL, common::TypeModifier::SW)] = [this] {
        SkipEqual<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SKIP_EQUAL, common::TypeModifier::R)] = [this] {
        SkipEqual<common::Real>();
    };

    // JnS
    instructions_handlers_[InstructionByte(common::OpCode::JUMPNSTORE, common::TypeModifier::W)] = [this] {
        JumpAndStore();
    };

}

/*
 * Интерфейс
*/

void Processor::Run() {
    SetStatus(true);

    while (is_running_) {
        if (is_waiting_input_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        ExecuteInstruction();
    }
}

void Processor::Step() {
    SetStatus(true);
    ExecuteInstruction();
    SetStatus(false);
}

void Processor::Stop() {
    is_waiting_input_ = false;
    SetStatus(false);
}

void Processor::Reset() {
    SetAccumulator(0);
    SetAuxiliary(0);
    SetInstructionPointer(0);

    SetStatus(false);
}

const Registers& Processor::GetRegisters() const {
    return registers_;
}

void Processor::SetObserver(ProcessorObserver* observer = nullptr) {
    observer_ = observer;
}

void Processor::SetIo(ProcessorIo* io) {
    io_ = io;
}

const common::Bytes& Processor::GetAccumulator() const {
    return registers_.accumulator;
}

const common::Bytes& Processor::GetAuxiliary() const {
    return registers_.auxiliary;
}

const common::DoubleByte& Processor::GetInstructionPointer() const {
    return registers_.instruction_pointer;
}

void Processor::SetInstructionPointer(const common::DoubleByte value) {
    registers_.instruction_pointer = value;

    if (observer_) {
        observer_->OnRegisterIpChanged(registers_.instruction_pointer);
    }

    if (value >= memory_.Size()) {
        SetStatus(false);
    }
}

const bool& Processor::IsRunning() const {
    return is_running_;
}

void Processor::ExecuteInstruction() {
    common::Byte code;
    common::Bytes argument;

    try {
        std::tie(code, argument) = memory_.ReadInstruction(registers_.instruction_pointer);
    } catch ([[maybe_unused]] std::out_of_range& e) {
        SetStatus(false);
        return;
    }

    const common::Byte handler = code & 0b11111100;
    const common::ArgModifier arg_modifier = argument_modifiers_[code & 0b00000011];

    if (arg_modifier == common::ArgModifier::REF) {
        SetAuxiliary(memory_.ReadArgument(static_cast<common::Word>(argument)));
    } else if (arg_modifier == common::ArgModifier::REF_REF) {
        SetAuxiliary(memory_.ReadArgument(static_cast<common::Word>(memory_.ReadArgument(static_cast<common::Word>(argument)))));
    } else {
        SetAuxiliary(argument);
    }

    try {
        instructions_handlers_[handler]();
    } catch ([[maybe_unused]] std::bad_function_call &e) {
        SetStatus(false);
        throw std::runtime_error(std::format("Error while executing: instruction {} at {} undefined",
                                                 std::bitset<8>(handler).to_string(),
                                                 registers_.instruction_pointer));
    }
}

inline void Processor::NextInstruction() {
    SetInstructionPointer(registers_.instruction_pointer + 1);
}

template <typename T>
common::Type Processor::TypeIo() {
    common::Type type = {};

    if (typeid(T) == typeid(common::Byte)) {
        type = common::Type::BYTE;
    } else if (typeid(T) == typeid(common::Word)) {
        type = common::Type::WORD;
    } else if (typeid(T) == typeid(common::SignedWord)) {
        type = common::Type::SIGNED_WORD;
    } else if (typeid(T) == typeid(common::Real)) {
        type = common::Type::REAL;
    }

    return type;
}

void Processor::SetStatus(const bool is_running) {
    is_running_ = is_running;
    if (observer_) {
        observer_->OnStatusChanged(is_running_);
    }
}

/*
 *  Реализация инструкций
 */

void Processor::Nope() {
    NextInstruction();
}

/*
 *  Арифметические
 */

template <typename T>
void Processor::Add() {
    SetAccumulator(static_cast<T>(static_cast<T>(registers_.accumulator) + static_cast<T>(registers_.auxiliary)));
    NextInstruction();
}

template <typename T>
void Processor::Sub() {
    SetAccumulator(static_cast<T>(static_cast<T>(registers_.accumulator) - static_cast<T>(registers_.auxiliary)));
    NextInstruction();
}

template <typename T>
void Processor::Mul() {
    SetAccumulator(static_cast<T>(static_cast<T>(registers_.accumulator) * static_cast<T>(registers_.auxiliary)));
    NextInstruction();
}

template <typename T>
void Processor::Div() {
    T value = static_cast<T>(registers_.auxiliary);

    if (value == 0) {
        throw std::runtime_error("Error: Division by zero");
    }

    SetAccumulator(static_cast<T>(static_cast<T>(registers_.accumulator) / value));
    NextInstruction();
}

template <typename T>
void Processor::Mod() {
    if (static_cast<T>(registers_.auxiliary) == 0) {
        throw std::runtime_error("Error: Modulo by zero");
    }
    SetAccumulator(static_cast<T>(static_cast<T>(registers_.accumulator) % static_cast<T>(registers_.auxiliary)));
    NextInstruction();
}

/*
 *  Работа с памятью
 */

template <typename T>
void Processor::Load() {
    SetAccumulator(static_cast<T>(registers_.auxiliary));
    NextInstruction();
}

void Processor::Store() {
    const auto address = static_cast<common::Word>(registers_.auxiliary);

    memory_.WriteArgument(registers_.accumulator, address);

    if (observer_) {
        observer_->OnMemoryChanged(address);
    }

    NextInstruction();
}

/*
 *  Ввод/вывод
 */

template <typename T>
void Processor::Input() {
    if (io_) {
        is_waiting_input_ = true;

        io_->InputAsync(TypeIo<T>(), [this](const common::Bytes bytes) {
            is_waiting_input_ = false;
            registers_.accumulator = bytes;
            NextInstruction();
        });
    }
}

template <typename T>
void Processor::Output() {
    common::Type type = TypeIo<T>();

    if (io_) {
        io_->Output(registers_.accumulator, type);
    }

    NextInstruction();
}

/*
 *  Ветвление
 */

void Processor::Jump() {
    SetInstructionPointer(static_cast<common::Word>(registers_.auxiliary));
}

template <class T>
void Processor::SkipLower() {
    if (static_cast<T>(registers_.accumulator) < static_cast<T>(registers_.auxiliary)) {
        SetInstructionPointer(registers_.instruction_pointer + 2);
    } else {
        NextInstruction();
    }
}

template <class T>
void Processor::SkipGreater() {
    if (static_cast<T>(registers_.accumulator) > static_cast<T>(registers_.auxiliary)) {
        SetInstructionPointer(registers_.instruction_pointer + 2);
    } else {
        NextInstruction();
    }
}

template <class T>
void Processor::SkipEqual() {
    if (static_cast<T>(registers_.accumulator) == static_cast<T>(registers_.auxiliary)) {
        SetInstructionPointer(registers_.instruction_pointer + 2);
    } else {
        NextInstruction();
    }
}

void Processor::JumpAndStore() {
    const auto address = static_cast<common::Word>(registers_.auxiliary);
    memory_.WriteArgument(common::Bytes(registers_.instruction_pointer + 1), address);
    if (observer_) {
        observer_->OnMemoryChanged(address);
    }
    SetInstructionPointer(address + 1);
}

