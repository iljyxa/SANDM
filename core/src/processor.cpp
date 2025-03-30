#include "../../core/include/processor.hpp"

#include <bitset>

Processor::Processor(MemoryManager& memory, ProcessorObserver* observer, ProcessorIo* io) :
    memory_(memory),
    observer_(observer),
    io_(io),
    is_running_(false) {

    // nope
    instructions_handlers_[InstructionByte(common::OpCode::NOP, common::TypeModifier::C)] = [this] {
        Nope();
    };
    instructions_handlers_[InstructionByte(common::OpCode::NOP, common::TypeModifier::W)] = [this] {
        Nope();
    };
    instructions_handlers_[InstructionByte(common::OpCode::NOP, common::TypeModifier::SW)] = [this] {
        Nope();
    };
    instructions_handlers_[InstructionByte(common::OpCode::NOP, common::TypeModifier::R)] = [this] {
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

    // jmp
    instructions_handlers_[InstructionByte(common::OpCode::JMP, common::TypeModifier::C)] = [this] {
        Jmp<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::JMP, common::TypeModifier::W)] = [this] {
        Jmp<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::JMP, common::TypeModifier::SW)] = [this] {
        Jmp<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::JMP, common::TypeModifier::R)] = [this] {
        Jmp<common::Real>();
    };

    // jnz
    instructions_handlers_[InstructionByte(common::OpCode::JNZ, common::TypeModifier::C)] = [this] {
        Jnz<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::JNZ, common::TypeModifier::W)] = [this] {
        Jnz<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::JNZ, common::TypeModifier::SW)] = [this] {
        Jnz<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::JNZ, common::TypeModifier::R)] = [this] {
        Jnz<common::Real>();
    };

    // jgz
    instructions_handlers_[InstructionByte(common::OpCode::JGZ, common::TypeModifier::C)] = [this] {
        Jgz<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::JGZ, common::TypeModifier::W)] = [this] {
        Jgz<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::JGZ, common::TypeModifier::SW)] = [this] {
        Jgz<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::JGZ, common::TypeModifier::R)] = [this] {
        Jgz<common::Real>();
    };

    // set
    instructions_handlers_[InstructionByte(common::OpCode::SET, common::TypeModifier::C)] = [this] {
        Set<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SET, common::TypeModifier::W)] = [this] {
        Set<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SET, common::TypeModifier::SW)] = [this] {
        Set<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SET, common::TypeModifier::R)] = [this] {
        Set<common::Real>();
    };

    // save
    instructions_handlers_[InstructionByte(common::OpCode::SAVE, common::TypeModifier::C)] = [this] {
        Save<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SAVE, common::TypeModifier::W)] = [this] {
        Save<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SAVE, common::TypeModifier::SW)] = [this] {
        Save<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::SAVE, common::TypeModifier::R)] = [this] {
        Save<common::Real>();
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

    // read
    instructions_handlers_[InstructionByte(common::OpCode::READ, common::TypeModifier::C)] = [this] {
        Read<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::READ, common::TypeModifier::W)] = [this] {
        Read<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::READ, common::TypeModifier::SW)] = [this] {
        Read<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::READ, common::TypeModifier::R)] = [this] {
        Read<common::Real>();
    };

    // write
    instructions_handlers_[InstructionByte(common::OpCode::WRITE, common::TypeModifier::C)] = [this] {
        Write<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::WRITE, common::TypeModifier::W)] = [this] {
        Write<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::WRITE, common::TypeModifier::SW)] = [this] {
        Write<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::WRITE, common::TypeModifier::R)] = [this] {
        Write<common::Real>();
    };

    // page
    instructions_handlers_[InstructionByte(common::OpCode::PAGE, common::TypeModifier::C)] = [this] {
        Page<common::Byte>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::PAGE, common::TypeModifier::W)] = [this] {
        Page<common::Word>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::PAGE, common::TypeModifier::SW)] = [this] {
        Page<common::SignedWord>();
    };
    instructions_handlers_[InstructionByte(common::OpCode::PAGE, common::TypeModifier::R)] = [this] {
        Page<common::Real>();
    };
}

/*
 * Интерфейс
*/

void Processor::Run() {
    SetStatus(true);

    while (is_running_) {
        ExecuteInstruction();
    }
}

void Processor::Step() {
    SetStatus(true);
    ExecuteInstruction();
    SetStatus(false);
}

void Processor::Stop() {
    SetStatus(false);
}

void Processor::Reset() {
    SetAccumulator(0);
    SetAuxiliary(0);
    SetInstructionPointer(0);
    SetPageTableIndex(0);

    SetStatus(false);
}

const Registers& Processor::GetRegisters() const {
    return registers;
}

void Processor::SetObserver(ProcessorObserver* observer = nullptr) {
    observer_ = observer;
}

void Processor::SetIo(ProcessorIo* io) {
    io_ = io;
}

const common::Bytes& Processor::GetAccumulator() const {
    return registers.accumulator;
}

const common::Bytes& Processor::GetAuxiliary() const {
    return registers.auxiliary;
}

const common::DoubleByte& Processor::GetInstructionPointer() const {
    return registers.instruction_pointer;
}

void Processor::SetInstructionPointer(const common::DoubleByte value) {
    registers.instruction_pointer = value;
    if (observer_) {
        observer_->OnRegisterIpChanged(registers.instruction_pointer);
    }
}

const common::Byte& Processor::GetPageTableIndex() const {
    return registers.page_table_index;
}

void Processor::SetPageTableIndex(const common::Byte value) {
    registers.page_table_index = value;
    if (observer_) {
        observer_->OnRegisterPtiChanged(registers.page_table_index);
    }
}

const bool& Processor::IsRunning() const {
    return is_running_;
}

void Processor::ExecuteInstruction() {
    common::Byte code;
    common::Bytes argument;

    try {
        std::tie(code, argument) = memory_.ReadInstruction(registers.instruction_pointer);
    } catch ([[maybe_unused]] std::out_of_range& e) {
        SetStatus(false);
        return;
    }

    const common::Byte handler = code & 0b11111110;
    const bool use_memory = code & 0b00000001;

    SetAuxiliary(use_memory ? memory_.ReadData(static_cast<common::DoubleByte>(argument)) : argument);
    try {
        instructions_handlers_[handler]();
    } catch ([[maybe_unused]] std::bad_function_call &e) {
        SetStatus(false);
        throw std::runtime_error(std::format("Error while executing: instruction {} at {} undefined",
                                                 std::bitset<8>(handler).to_string(),
                                                 registers.instruction_pointer));
    }


}

inline void Processor::NextInstruction() {
    SetInstructionPointer(registers.instruction_pointer + 1);
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
    SetAccumulator(static_cast<T>(static_cast<T>(registers.accumulator) + static_cast<T>(registers.auxiliary)));
    NextInstruction();
}

template <typename T>
void Processor::Sub() {
    SetAccumulator(static_cast<T>(static_cast<T>(registers.accumulator) - static_cast<T>(registers.auxiliary)));
    NextInstruction();
}

template <typename T>
void Processor::Mul() {
    SetAccumulator(static_cast<T>(static_cast<T>(registers.accumulator) * static_cast<T>(registers.auxiliary)));
    NextInstruction();
}

template <typename T>
void Processor::Div() {
    T value = static_cast<T>(registers.auxiliary);

    if (value == 0) {
        throw std::runtime_error("Error: Division by zero");
    }

    SetAccumulator(static_cast<T>(static_cast<T>(registers.accumulator) / value));
    NextInstruction();
}

template <typename T>
void Processor::Mod() {
    SetAccumulator(static_cast<T>(static_cast<T>(registers.accumulator) % static_cast<T>(registers.auxiliary)));

    if (observer_) {
        observer_->OnRegisterAccChanged(registers.accumulator);
    }

    NextInstruction();
}

/*
 *  Ветвление
 */

template <typename T>
void Processor::Jmp() {
    SetInstructionPointer(static_cast<T>(registers.auxiliary));
}

template <typename T>
void Processor::Jnz() {
    if (static_cast<int>(registers.accumulator)) {
        SetInstructionPointer(static_cast<T>(registers.auxiliary));
    } else {
        NextInstruction();
    }
}

template <typename T>
void Processor::Jgz() {
    if (static_cast<int>(registers.accumulator) > 0) {
        SetInstructionPointer(static_cast<T>(registers.auxiliary));
    } else {
        NextInstruction();
    }
}

template <typename T>
void Processor::Cpsg() {
    if (static_cast<T>(registers.accumulator) > static_cast<T>(registers.auxiliary)) {
        SetInstructionPointer(registers.instruction_pointer + 2);
    }
}

template <typename T>
void Processor::Cpse() {
    if (static_cast<T>(registers.accumulator) == static_cast<T>(registers.auxiliary)) {
        SetInstructionPointer(registers.instruction_pointer + 2);
    }
}

/*
 *  Работа с данными
 */

template <typename T>
void Processor::Set() {
    SetAccumulator(static_cast<T>(registers.auxiliary));
    NextInstruction();
}

template <typename T>
void Processor::Load() {
    SetAccumulator(memory_.ReadData(static_cast<T>(registers.auxiliary)));
    NextInstruction();
}

template <typename T>
void Processor::Page() {
    T page_table_index = static_cast<T>(registers.auxiliary);
    if (page_table_index >= std::numeric_limits<typeof(registers.page_table_index)>::max()) {
        throw std::invalid_argument(std::format("Page table index {} exceeds available memory.", page_table_index));
    }

    SetPageTableIndex(page_table_index);

    NextInstruction();
}

template <typename T>
void Processor::Save() {
    T virtual_address = static_cast<T>(registers.auxiliary);
    if (virtual_address >= common::PAGE_SIZE) {
        throw std::invalid_argument(std::format("Address {} exceeds available page memory.", virtual_address));
    }

    common::DoubleByte address = (registers.page_table_index + 1) * virtual_address;

    memory_.WriteData(address, registers.accumulator);

    if (observer_) {
        observer_->OnMemoryChanged(address);
    }

    NextInstruction();
}

/*
 *  Прерывания
 */

template <typename T>
void Processor::Read() {
    common::Type type = TypeIo<T>();

    if (io_) {
        io_->Input(registers.accumulator, type);
    }

    NextInstruction();
}

template <typename T>
void Processor::Write() {
    common::Type type = TypeIo<T>();

    if (io_) {
        io_->Output(registers.accumulator, type);
    }

    NextInstruction();
}
