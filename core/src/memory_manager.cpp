#include <limits>
#include <ranges>
#include <stdexcept>

#include "../../core/include/memory_manager.hpp"

void MemoryManager::Load(const common::ByteCode& byte_code) {
    instructions_.clear();
    arguments_.clear();

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

        address++;
    }

    instructions_.shrink_to_fit();
    arguments_.shrink_to_fit();
}

std::pair<common::Byte, common::Bytes> MemoryManager::ReadInstruction(const common::DoubleByte address) {
    if (address > instructions_.size() - 1 || instructions_.empty()) {
        throw std::out_of_range("Instruction address out of range.");
    }

    return std::make_pair(instructions_[address], arguments_[address]);
}

void MemoryManager::WriteInstruction(const common::Byte code, const common::Bytes argument,
                                      const common::DoubleByte address) {
    if (instructions_.size() < address + 1) {
        instructions_.resize(address + 1);
        arguments_.resize(address + 1);
    }

    instructions_[address] = code;
    arguments_[address] = argument;
}

void MemoryManager::WriteInstruction(const common::Byte code, const common::Bytes argument) {
    instructions_.push_back(code);
    arguments_.push_back(argument);
}

common::Bytes MemoryManager::ReadData(const common::DoubleByte address) {
    const common::Byte page_number = address / common::PAGE_SIZE;
    const common::Byte page_offset = address % common::PAGE_SIZE;

    if (!data_.contains(page_number)) {
        return {};
    }

    return data_[page_number][page_offset];
}

void MemoryManager::WriteData(const common::DoubleByte address, const common::Bytes value) {
    const common::Byte page_number = address / common::PAGE_SIZE;
    const common::Byte page_offset = address % common::PAGE_SIZE;

    if (!data_.contains(page_number)) {
        constexpr common::Page page{};
        data_[page_number] = page;
    }

    data_[page_number][page_offset] = value;
}

void MemoryManager::Reset() {
    data_.clear();
    instructions_.clear();
    arguments_.clear();
}

void MemoryManager::ResetData() {
    data_.clear();
}

void MemoryManager::ResetInstructions() {
    instructions_.clear();
    arguments_.clear();
}
