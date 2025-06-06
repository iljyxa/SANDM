
#include "core/memory_manager.hpp"

void MemoryManager::Load(const snm::ByteCode& byte_code) {
    instructions_.clear();
    arguments_.clear();
    arguments_original_.clear();

    instructions_.reserve(byte_code.size() / 5);
    arguments_.reserve(byte_code.size() / 5);

    if (byte_code.size() > std::numeric_limits<snm::Address>::max()) {
        throw std::invalid_argument("Command size exceeds available memory. Cannot load instructions.");
    }

    if (byte_code.size() % 5 != 0) {
        throw std::invalid_argument("Invalid bytecode format. Unable to parse.");
    }

    snm::Address address = 0;

    for (size_t i = 0; i < byte_code.size(); i += 5) {
        const snm::Byte code = byte_code[i];
        snm::Bytes argument;

        for (size_t j = 0; j < 4; ++j) {
            argument[j] = byte_code[i + 1 + j];
        }

        WriteInstruction(code, argument);
        arguments_original_.push_back(argument);
        address++;
    }

    instructions_.shrink_to_fit();
    arguments_.shrink_to_fit();
    arguments_original_.shrink_to_fit();
    arguments_original_ = arguments_;
}

std::pair<snm::Byte, snm::Bytes> MemoryManager::ReadInstruction(const snm::Address address) {
    if (address > instructions_.size() - 1 || instructions_.empty()) {
        throw std::out_of_range("Instruction address out of range.");
    }

    return std::make_pair(instructions_[address], arguments_[address]);
}

void MemoryManager::WriteInstruction(const snm::Byte code, const snm::Bytes argument,
                                     const snm::Address address) {

    if (instructions_.size() <= address) {
        instructions_.resize(address + 1);
        arguments_.resize(address + 1);
    }

    instructions_[address] = code;
    arguments_[address] = argument;
}

void MemoryManager::WriteInstruction(const snm::Byte code, const snm::Bytes argument) {
    instructions_.push_back(code);
    arguments_.push_back(argument);
}

void MemoryManager::WriteArgument(const snm::Bytes argument, const snm::Address address) {
    if (arguments_.size() <= address) {
        arguments_.resize(address + 1);
    }

    arguments_[address] = argument;
}

snm::Bytes MemoryManager::ReadArgument(const snm::Address address) const {
    if (arguments_.size() <= address || arguments_.empty()) {
        return {};
    }

    return arguments_[address];
}

void MemoryManager::Reset() {
    instructions_.clear();
    arguments_.clear();
    arguments_original_.clear();
}

void MemoryManager::ResetData() {
    arguments_ = arguments_original_;
}

size_t MemoryManager::Size() const {
    return instructions_.size();
}
