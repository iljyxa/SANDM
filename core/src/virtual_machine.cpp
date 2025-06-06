
#include "core/virtual_machine.hpp"

VirtualMachine::VirtualMachine(ProcessorIo* processor_io) :
    memory_manager_(std::make_unique<MemoryManager>()),
    processor_(std::make_unique<Processor>(*memory_manager_, nullptr, processor_io ? processor_io : this)) {
}

void VirtualMachine::Load(const snm::ByteCode& byte_code) {
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

snm::Bytes VirtualMachine::ReadMemory(const snm::Address& address) {
    return memory_manager_->ReadArgument(address);
}

void VirtualMachine::WriteMemory(const snm::Address& address, const snm::Bytes& data) {
    memory_manager_->WriteArgument(data, address);
}

bool VirtualMachine::IsRunning() {
    return processor_->IsRunning();
}

const snm::ProcessorState& VirtualMachine::GetState() {
    return processor_->GetState();
}

Registers VirtualMachine::GetRegisters() {
    return processor_->GetRegisters();
}

void VirtualMachine::SetInstructionPointer(const snm::Address value) {
    processor_->SetInstructionPointer(value);
}

void VirtualMachine::SetAccumulator(const snm::Byte value) {
    processor_->SetAccumulator(value);
}

void VirtualMachine::SetAccumulator(const snm::Word value) {
    processor_->SetAccumulator(value);
}

void VirtualMachine::SetAccumulator(const snm::SignedWord value) {
    processor_->SetAccumulator(value);
}

void VirtualMachine::SetAccumulator(const snm::Real value) {
    processor_->SetAccumulator(value);
}

void VirtualMachine::SetAuxiliary(const snm::Byte value) {
    processor_->SetAuxiliary(value);
}

void VirtualMachine::SetAuxiliary(const snm::Word value) {
    processor_->SetAuxiliary(value);
}

void VirtualMachine::SetAuxiliary(const snm::SignedWord value) {
    processor_->SetAuxiliary(value);
}

void VirtualMachine::SetAuxiliary(const snm::Real value) {
    processor_->SetAuxiliary(value);
}

void VirtualMachine::Output(snm::Bytes& bytes, snm::Type& type) {
    std::cout << BytesToString(bytes, type) << std::endl;
}

void VirtualMachine::InputAsync(const snm::Type type, const InputCallback callback) {
    std::string input_string;
    std::cin >> input_string;
    callback(BytesFromString(input_string, type));
}

void VirtualMachine::SetProcessorObserver(ProcessorObserver* observer) const {
    processor_->SetObserver(observer);
}

void VirtualMachine::SetProcessorIo(ProcessorIo* processor_io) const {
    processor_->SetIo(processor_io);
}

std::string VirtualMachine::BytesToString(const snm::Bytes& bytes, const snm::Type& type) {
    switch (type) {
    case snm::Type::BYTE:
        return {static_cast<char>(bytes)};
    case snm::Type::WORD:
        return std::to_string(static_cast<snm::Word>(bytes));
    case snm::Type::SIGNED_WORD:
        return std::to_string(static_cast<snm::SignedWord>(bytes));
    case snm::Type::REAL:
        return std::to_string(static_cast<snm::Real>(bytes));
    default:
        throw std::invalid_argument("Invalid type");
    }
}

snm::Bytes VirtualMachine::BytesFromString(const std::string& string, const snm::Type& type) {
    switch (type) {
    case snm::Type::BYTE:
        return snm::Bytes(static_cast<snm::Byte>(std::stoul(string)));
    case snm::Type::WORD:
        return snm::Bytes(static_cast<snm::Word>(stoul(string)));
    case snm::Type::SIGNED_WORD:
        return snm::Bytes(std::stoi(string));
    case snm::Type::REAL:
        return snm::Bytes(std::stof(string));
    default:
        throw std::invalid_argument("Invalid type");
    }
}
