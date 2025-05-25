#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include <vector>
#include "common_definitions.hpp"

class MemoryManager {
public:
    MemoryManager() = default;

    void Load(const common::ByteCode &byte_code);

    void WriteInstruction(common::Byte code, common::Bytes argument, common::DoubleByte address);
    void WriteInstruction(common::Byte code, common::Bytes argument);
    std::pair<common::Byte, common::Bytes> ReadInstruction(common::DoubleByte address);

    void WriteArgument(common::Bytes argument, common::DoubleByte address);
    common::Bytes ReadArgument(common::DoubleByte address);

    void Reset();
    [[nodiscard]] size_t Size() const;

private:
    std::vector<common::Byte> instructions_;
    std::vector<common::Bytes> arguments_;
};

#endif
