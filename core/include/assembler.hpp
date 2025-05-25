#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "common_definitions.hpp"

struct Instruction {
    common::OpCode opcode = common::OpCode::NOPE;
    common::ArgModifier argument_modifier = common::ArgModifier::NONE;
    common::TypeModifier type_modifier = common::TypeModifier::SW;
    common::Bytes argument{};
    std::optional<std::string> using_label_name;
    std::optional<std::string> label_name;
    unsigned int line_number = 0;
};

class Assembler {
public:
    Assembler();

    common::ByteCode Compile(const std::string& source);
    std::pair<common::ByteCode, common::SourceToBytecodeMap> CompileWithDebugInfo(const std::string& source);
    std::vector<std::string> TestSource(const std::string& source);

private:
    std::unordered_map<std::string, common::OpCode> opcode_map_;
    std::unordered_map<std::string, common::TypeModifier> type_modifier_map_;
    std::unordered_map<std::string, common::ArgModifier> arg_modifier_map_;
    unsigned int line_number_;

    template <class T>
    T Exception(const std::string& message);
    template <class T>
    static T Exception(const std::string& message, unsigned int line_number);

    static std::string Trim(const std::string& str);
    static bool IsValidLabelName(const std::string& name);
    static bool IsNumberValidForType(common::Bytes bytes, common::TypeModifier type_modifier);
    common::Bytes ParseNumber(const std::string& str, const common::TypeModifier& type_modifier);
    std::string GetLine(std::istringstream& stream);
    Instruction GetInstruction(const std::string& line);
    [[nodiscard]] bool IsArgumentModifier(std::string& token) const;
    [[nodiscard]] bool IsTypeModifier(std::string& token) const;
    void ValidateStringNumber(const std::string& str);
    static std::string RemoveComment(const std::string& line);
    static std::string ToUpper(std::string& str);
    std::pair<common::ByteCode, common::SourceToBytecodeMap> CompileInternal(const std::string& source);
    std::tuple<std::vector<Instruction>, std::unordered_map<std::string, uint32_t>, std::vector<std::string>> ParseSource(
        const std::string& source);
};

#endif
