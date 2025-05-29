#include <limits>
#include <ranges>
#include <stdexcept>

#include "../../core/include/memory_manager.hpp"

void MemoryManager::Load(const common::ByteCode& byte_code) {
    instructions_.clear();
    arguments_.clear();
    arguments_original_.clear();

    instructions_.reserve(byte_code.size());
    arguments_.reserve(byte_code.size());

    if (byte_code.size() > std::numeric_limits<common::DoubleByte>::max()) {
        throw std::invalid_argument("Command size exceeds available memory. Cannot load instructions.");
    }

    if (byte_code.size() % 5 != 0) {
        throw std::invalid_argument("Invalid bytecode format. Unable to parse.");
    }

    common::DoubleByte address = 0;

    for (size_t i = 0; i < byte_code.size(); i += 5) {
        const common::Byte code = byte_code[i];
        common::Bytes argument;

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

std::pair<common::Byte, common::Bytes> MemoryManager::ReadInstruction(const common::DoubleByte address) {
    if (address > instructions_.size() - 1 || instructions_.empty()) {
        throw std::out_of_range("Instruction address out of range.");
    }

    return std::make_pair(instructions_[address], arguments_[address]);
}

void MemoryManager::WriteInstruction(const common::Byte code, const common::Bytes argument,
                                     const common::DoubleByte address) {
    if (address >= common::CODE_MEMORY_SIZE) {
        throw std::invalid_argument(std::format("Address {} exceeds available memory.", address));
    }

    if (instructions_.size() <= address) {
        instructions_.resize(address);
        arguments_.resize(address);
    }

    instructions_[address] = code;
    arguments_[address] = argument;
}

void MemoryManager::WriteInstruction(const common::Byte code, const common::Bytes argument) {
    instructions_.push_back(code);
    arguments_.push_back(argument);
}

void MemoryManager::WriteArgument(const common::Bytes argument, const common::DoubleByte address) {
    if (address >= common::CODE_MEMORY_SIZE) {
        throw std::invalid_argument(std::format("Address {} exceeds available memory.", address));
    }

    if (instructions_.size() <= address) {
        instructions_.resize(address);
        arguments_.resize(address);
    }

    arguments_[address] = argument;
}

common::Bytes MemoryManager::ReadArgument(common::DoubleByte address) {
    if (address >= common::CODE_MEMORY_SIZE) {
        throw std::invalid_argument(std::format("Address {} exceeds available memory.", address));
    }

    if (instructions_.size() <= address || instructions_.empty()) {
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
