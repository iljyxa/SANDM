#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>
#include "common_definitions.hpp"

class MemoryManager {
public:
    MemoryManager() = default;

    void Load(const common::ByteCode &byte_code);

    std::pair<common::Byte, common::Bytes> ReadInstruction(common::DoubleByte address);

    void WriteInstruction(common::Byte code, common::Bytes argument, common::DoubleByte address);
    void WriteInstruction(common::Byte code, common::Bytes argument);

    common::Bytes ReadData(common::DoubleByte address);
    void WriteData(common::DoubleByte address, common::Bytes value);
    void Reset();
    void ResetData();
    void ResetInstructions();

    std::vector<common::Byte> instructions_;
    std::vector<common::Bytes> arguments_;

private:

    std::unordered_map<common::Byte, common::Page> data_;
};

#endif
