#include "core/processor.hpp"

Processor::Processor(MemoryManager& memory, ProcessorObserver* observer, ProcessorIo* io) :
    memory_(memory),
    observer_(observer),
    io_(io),
    state_(snm::ProcessorState::STOPPED) {
    argument_modifiers_[static_cast<uint8_t>(snm::ArgModifier::NONE)] = snm::ArgModifier::NONE;
    argument_modifiers_[static_cast<uint8_t>(snm::ArgModifier::REF)] = snm::ArgModifier::REF;
    argument_modifiers_[static_cast<uint8_t>(snm::ArgModifier::REF_REF)] = snm::ArgModifier::REF_REF;

    // nope
    instructions_handlers_[InstructionByte(snm::OpCode::NOPE, snm::TypeModifier::C)] = [this] {
        Nope();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::NOPE, snm::TypeModifier::W)] = [this] {
        Nope();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::NOPE, snm::TypeModifier::SW)] = [this] {
        Nope();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::NOPE, snm::TypeModifier::R)] = [this] {
        Nope();
    };

    // add
    instructions_handlers_[InstructionByte(snm::OpCode::ADD, snm::TypeModifier::C)] = [this] {
        Add<snm::Byte>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::ADD, snm::TypeModifier::W)] = [this] {
        Add<snm::Word>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::ADD, snm::TypeModifier::SW)] = [this] {
        Add<snm::SignedWord>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::ADD, snm::TypeModifier::R)] = [this] {
        Add<snm::Real>();
    };

    // sub
    instructions_handlers_[InstructionByte(snm::OpCode::SUB, snm::TypeModifier::C)] = [this] {
        Sub<snm::Byte>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SUB, snm::TypeModifier::W)] = [this] {
        Sub<snm::Word>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SUB, snm::TypeModifier::SW)] = [this] {
        Sub<snm::SignedWord>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SUB, snm::TypeModifier::R)] = [this] {
        Sub<snm::Real>();
    };

    // mul
    instructions_handlers_[InstructionByte(snm::OpCode::MUL, snm::TypeModifier::C)] = [this] {
        Mul<snm::Byte>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::MUL, snm::TypeModifier::W)] = [this] {
        Mul<snm::Word>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::MUL, snm::TypeModifier::SW)] = [this] {
        Mul<snm::SignedWord>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::MUL, snm::TypeModifier::R)] = [this] {
        Mul<snm::Real>();
    };

    // div
    instructions_handlers_[InstructionByte(snm::OpCode::DIV, snm::TypeModifier::C)] = [this] {
        Div<snm::Byte>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::DIV, snm::TypeModifier::W)] = [this] {
        Div<snm::Word>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::DIV, snm::TypeModifier::SW)] = [this] {
        Div<snm::SignedWord>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::DIV, snm::TypeModifier::R)] = [this] {
        Div<snm::Real>();
    };

    // mod
    instructions_handlers_[InstructionByte(snm::OpCode::MOD, snm::TypeModifier::C)] = [this] {
        Mod<snm::Byte>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::MOD, snm::TypeModifier::W)] = [this] {
        Mod<snm::Word>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::MOD, snm::TypeModifier::SW)] = [this] {
        Mod<snm::SignedWord>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::MOD, snm::TypeModifier::R)] = [this] {
        Mod<snm::Real>();
    };

    // load
    instructions_handlers_[InstructionByte(snm::OpCode::LOAD, snm::TypeModifier::C)] = [this] {
        Load<snm::Byte>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::LOAD, snm::TypeModifier::W)] = [this] {
        Load<snm::Word>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::LOAD, snm::TypeModifier::SW)] = [this] {
        Load<snm::SignedWord>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::LOAD, snm::TypeModifier::R)] = [this] {
        Load<snm::Real>();
    };

    // store
    instructions_handlers_[InstructionByte(snm::OpCode::STORE, snm::TypeModifier::W)] = [this] {
        Store();
    };

    // input
    instructions_handlers_[InstructionByte(snm::OpCode::INPUT, snm::TypeModifier::C)] = [this] {
        Input<snm::Byte>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::INPUT, snm::TypeModifier::W)] = [this] {
        Input<snm::Word>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::INPUT, snm::TypeModifier::SW)] = [this] {
        Input<snm::SignedWord>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::INPUT, snm::TypeModifier::R)] = [this] {
        Input<snm::Real>();
    };

    // output
    instructions_handlers_[InstructionByte(snm::OpCode::OUTPUT, snm::TypeModifier::C)] = [this] {
        Output<snm::Byte>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::OUTPUT, snm::TypeModifier::W)] = [this] {
        Output<snm::Word>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::OUTPUT, snm::TypeModifier::SW)] = [this] {
        Output<snm::SignedWord>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::OUTPUT, snm::TypeModifier::R)] = [this] {
        Output<snm::Real>();
    };

    // jump
    instructions_handlers_[InstructionByte(snm::OpCode::JUMP, snm::TypeModifier::W)] = [this] {
        Jump();
    };

    // skiplower
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_LOWER, snm::TypeModifier::C)] = [this] {
        SkipLower<snm::Byte>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_LOWER, snm::TypeModifier::W)] = [this] {
        SkipLower<snm::Word>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_LOWER, snm::TypeModifier::SW)] = [this] {
        SkipLower<snm::SignedWord>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_LOWER, snm::TypeModifier::R)] = [this] {
        SkipLower<snm::Real>();
    };

    // skipgreater
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_GREATER, snm::TypeModifier::C)] = [this] {
        SkipGreater<snm::Byte>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_GREATER, snm::TypeModifier::W)] = [this] {
        SkipGreater<snm::Word>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_GREATER, snm::TypeModifier::SW)] = [this] {
        SkipGreater<snm::SignedWord>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_GREATER, snm::TypeModifier::R)] = [this] {
        SkipGreater<snm::Real>();
    };

    // skipequal
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_EQUAL, snm::TypeModifier::C)] = [this] {
        SkipEqual<snm::Byte>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_EQUAL, snm::TypeModifier::W)] = [this] {
        SkipEqual<snm::Word>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_EQUAL, snm::TypeModifier::SW)] = [this] {
        SkipEqual<snm::SignedWord>();
    };
    instructions_handlers_[InstructionByte(snm::OpCode::SKIP_EQUAL, snm::TypeModifier::R)] = [this] {
        SkipEqual<snm::Real>();
    };

    // JnS
    instructions_handlers_[InstructionByte(snm::OpCode::JUMPNSTORE, snm::TypeModifier::W)] = [this] {
        JumpAndStore();
    };

    // Halt
    instructions_handlers_[InstructionByte(snm::OpCode::HALT, snm::TypeModifier::C)] = [this] {
        Halt();
    };

}

/*
 * Интерфейс
*/

void Processor::Run() {
    SetState(snm::ProcessorState::RUNNING);

    while (IsRunning()) {
        if (state_ == snm::ProcessorState::PAUSED_BY_IO) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        ExecuteInstruction();
    }

    SetState(snm::ProcessorState::STOPPED);
}

void Processor::Step() {
    SetState(snm::ProcessorState::RUNNING);
    ExecuteInstruction();
    if (IsRunning()) {
        SetState(snm::ProcessorState::PAUSED);
    }
}

void Processor::Stop() {
    SetState(snm::ProcessorState::STOPPED);
}

void Processor::Reset() {
    SetAccumulator(0);
    SetAuxiliary(0);
    SetInstructionPointer(0);

    SetState(snm::ProcessorState::STOPPED);
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

const snm::Bytes& Processor::GetAccumulator() const {
    return registers_.accumulator;
}

const snm::Bytes& Processor::GetAuxiliary() const {
    return registers_.auxiliary;
}

const snm::Address& Processor::GetInstructionPointer() const {
    return registers_.instruction_pointer;
}

void Processor::SetInstructionPointer(const snm::Address value) {
    registers_.instruction_pointer = value;

    if (observer_) {
        observer_->OnRegisterIpChanged(registers_.instruction_pointer);
    }

    if (value >= memory_.Size()) {
        SetState(snm::ProcessorState::STOPPED);
    }
}

const snm::ProcessorState& Processor::GetState() const {
    return state_;
}

void Processor::ExecuteInstruction() {
    snm::Byte code;
    snm::Bytes argument;

    try {
        std::tie(code, argument) = memory_.ReadInstruction(registers_.instruction_pointer);
    } catch ([[maybe_unused]] std::out_of_range& e) {
        SetState(snm::ProcessorState::STOPPED);
        return;
    }

    snm::Byte handler;

    if (code == std::numeric_limits<snm::Byte>::max()) {
        handler = code;
    } else {
        handler = code & 0b11111100;
        const snm::ArgModifier arg_modifier = argument_modifiers_[code & 0b00000011];

        if (arg_modifier == snm::ArgModifier::REF) {
            SetAuxiliary(memory_.ReadArgument(static_cast<snm::Word>(argument)));
        } else if (arg_modifier == snm::ArgModifier::REF_REF) {
            SetAuxiliary(
                memory_.ReadArgument(static_cast<snm::Word>(memory_.ReadArgument(static_cast<snm::Word>(argument)))));
        } else {
            SetAuxiliary(argument);
        }
    }

    try {
        instructions_handlers_[handler]();
    } catch ([[maybe_unused]] std::bad_function_call &e) {
        SetState(snm::ProcessorState::STOPPED);
        throw std::runtime_error(std::format("Error while executing: instruction {} at {} undefined",
                                                 std::bitset<8>(handler).to_string(),
                                                 registers_.instruction_pointer));
    }
}

inline void Processor::NextInstruction() {
    SetInstructionPointer(registers_.instruction_pointer + 1);
}

template <typename T>
snm::Type Processor::TypeIo() {
    snm::Type type = {};

    if (typeid(T) == typeid(snm::Byte)) {
        type = snm::Type::BYTE;
    } else if (typeid(T) == typeid(snm::Word)) {
        type = snm::Type::WORD;
    } else if (typeid(T) == typeid(snm::SignedWord)) {
        type = snm::Type::SIGNED_WORD;
    } else if (typeid(T) == typeid(snm::Real)) {
        type = snm::Type::REAL;
    }

    return type;
}

void Processor::SetState(const snm::ProcessorState state) {
    if (state_ != state) {
        if (state_ == snm::ProcessorState::PAUSED_BY_IO && state == snm::ProcessorState::PAUSED) {
            // Обработка не требуется
        } else {
            state_ = state;

            if (observer_) {
                observer_->OnStateChanged(state_);
            }
        }
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

    if (value == static_cast<T>(0)) {
        throw std::runtime_error("Error: Division by zero");
    }

    SetAccumulator(static_cast<T>(static_cast<T>(registers_.accumulator) / value));
    NextInstruction();
}

template <typename T>
void Processor::Mod() {
    const T lhs = static_cast<T>(registers_.accumulator);
    const T rhs = static_cast<T>(registers_.auxiliary);

    if (rhs == static_cast<T>(0)) {
        throw std::runtime_error("Error: Modulo by zero");
    }

    T result;

    if constexpr (std::is_same_v<T, snm::Real>) {
        result = ::fmodf(lhs, rhs);
    } else {
        result = lhs % rhs;
    }

    SetAccumulator(result);
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
    const auto address = static_cast<snm::Word>(registers_.auxiliary);

    if (address >= snm::CODE_MEMORY_SIZE) {
        throw std::out_of_range(std::format("IP {}: Address {} exceeds available memory.", std::to_string(registers_.instruction_pointer), std::to_string(address)));
    }

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
        SetState(snm::ProcessorState::PAUSED_BY_IO);

        io_->InputRequest(TypeIo<T>(), [this](const snm::Bytes bytes) {
            if (state_ == snm::ProcessorState::PAUSED_BY_IO) {
                SetState(snm::ProcessorState::RUNNING);
                registers_.accumulator = bytes;
                NextInstruction();
            }
        });
    }
}

template <typename T>
void Processor::Output() {
    const snm::Type type = TypeIo<T>();

    if (io_) {
        io_->OutputRequest(registers_.accumulator, type);
    }

    NextInstruction();
}

/*
 *  Ветвление
 */

void Processor::Jump() {
    SetInstructionPointer(static_cast<snm::Word>(registers_.auxiliary));
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
    const auto address = static_cast<snm::Word>(registers_.auxiliary);
    memory_.WriteArgument(snm::Bytes(registers_.instruction_pointer + 1), address);
    if (observer_) {
        observer_->OnMemoryChanged(address);
    }
    SetInstructionPointer(address + 1);
}

void Processor::Halt() {
    Stop();
}
