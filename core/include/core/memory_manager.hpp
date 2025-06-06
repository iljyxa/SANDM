#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

// ReSharper disable once CppUnusedIncludeDirective
#include <limits>
// ReSharper disable once CppUnusedIncludeDirective
#include <stdexcept>
#include <vector>

#include "core/common_definitions.hpp"

class MemoryManager {
public:
    MemoryManager() = default;

    void Load(const snm::ByteCode& byte_code);

    void WriteInstruction(snm::Byte code, snm::Bytes argument, snm::Address address);
    void WriteInstruction(snm::Byte code, snm::Bytes argument);
    std::pair<snm::Byte, snm::Bytes> ReadInstruction(snm::Address address);

    void WriteArgument(snm::Bytes argument, snm::Address address);
    [[nodiscard]] snm::Bytes ReadArgument(snm::Address address) const;

    void Reset();
    void ResetData();
    [[nodiscard]] size_t Size() const;

private:
    std::vector<snm::Byte> instructions_;
    std::vector<snm::Bytes> arguments_;
    std::vector<snm::Bytes> arguments_original_;
};

#endif
