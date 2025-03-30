#include <iostream>

#include "../include/virtual_machine.hpp"

VirtualMachine::VirtualMachine(ProcessorIo* processor_io) :
    memory_manager_(std::make_unique<MemoryManager>()),
    processor_(std::make_unique<Processor>(*memory_manager_, nullptr, processor_io ? processor_io : this)) {
}

void VirtualMachine::Load(const common::ByteCode& byte_code) {
    memory_manager_->Load(byte_code);
}

void VirtualMachine::Reset() {
    if (processor_->IsRunning()) {
        processor_->Stop();
    }

    processor_->Reset();
    memory_manager_->Reset();
}

void VirtualMachine::Run() {
    processor_->Run();
}

void VirtualMachine::Stop() {
    processor_->Stop();
}

void VirtualMachine::Step() {
    processor_->Step();
}

common::Bytes VirtualMachine::ReadMemory(const common::Byte& virtual_address) {
    const common::DoubleByte address = common::PAGE_SIZE * processor_->GetPageTableIndex() + virtual_address;
    return memory_manager_->ReadData(address);
}

void VirtualMachine::WriteMemory(const common::Byte& virtual_address, const common::Bytes& data) {
    const common::DoubleByte address = common::PAGE_SIZE * processor_->GetPageTableIndex() + virtual_address;
    memory_manager_->WriteData(address, data);
}

bool VirtualMachine::IsRunning() {
    return processor_->IsRunning();
}

Registers VirtualMachine::GetRegisters() {
    return processor_->GetRegisters();
}

void VirtualMachine::SetInstructionPointer(const common::DoubleByte value) {
    processor_->SetInstructionPointer(value);
}

void VirtualMachine::SetPageTableIndex(const common::Byte value) {
    processor_->SetPageTableIndex(value);
}

void VirtualMachine::SetAccumulator(const common::Byte value) {
    processor_->SetAccumulator(value);
}

void VirtualMachine::SetAccumulator(const common::Word value) {
    processor_->SetAccumulator(value);
}

void VirtualMachine::SetAccumulator(const common::SignedWord value) {
    processor_->SetAccumulator(value);
}

void VirtualMachine::SetAccumulator(const common::Real value) {
    processor_->SetAccumulator(value);
}

void VirtualMachine::SetAuxiliary(const common::Byte value) {
    processor_->SetAuxiliary(value);
}

void VirtualMachine::SetAuxiliary(const common::Word value) {
    processor_->SetAuxiliary(value);
}

void VirtualMachine::SetAuxiliary(const common::SignedWord value) {
    processor_->SetAuxiliary(value);
}

void VirtualMachine::SetAuxiliary(const common::Real value) {
    processor_->SetAuxiliary(value);
}

void VirtualMachine::Output(common::Bytes& bytes, common::Type& type) {
    std::cout << BytesToString(bytes, type) << std::endl;
}

void VirtualMachine::Input(common::Bytes& bytes, common::Type& type) {
    std::string input_string;
    std::cin >> input_string;

    bytes = BytesFromString(input_string, type);
}

void VirtualMachine::SetProcessorObserver(ProcessorObserver* observer) const {
    processor_->SetObserver(observer);
}

void VirtualMachine::SetProcessorIo(ProcessorIo* processor_io) const {
    processor_->SetIo(processor_io);
}

std::string VirtualMachine::BytesToString(const common::Bytes& bytes, const common::Type& type) {
    switch (type) {
    case common::Type::BYTE:
        return std::basic_string(1, static_cast<char>(bytes));
    case common::Type::WORD:
        return std::to_string(static_cast<common::Word>(bytes));
    case common::Type::SIGNED_WORD:
        return std::to_string(static_cast<common::SignedWord>(bytes));
    case common::Type::REAL:
        return std::to_string(static_cast<common::Real>(bytes));
    default:
        throw std::invalid_argument("Invalid type");
    }
}

common::Bytes VirtualMachine::BytesFromString(const std::string& string, const common::Type& type) {
    switch (type) {
    case common::Type::BYTE:
        return common::Bytes(static_cast<common::Byte>(std::stoul(string)));
    case common::Type::WORD:
        return common::Bytes(static_cast<common::Word>(stoul(string)));
    case common::Type::SIGNED_WORD:
        return common::Bytes(std::stoi(string));
    case common::Type::REAL:
        return common::Bytes(std::stof(string));
    default:
        throw std::invalid_argument("Invalid type");
    }
}
