#ifndef COMMON_DEFINITIONS_HPP
#define COMMON_DEFINITIONS_HPP

#include <limits>
#include <set>
#include <vector>
#include "bytes.hpp"

namespace common {
    enum class OpCode {
        NOP = 0, ADD, SUB, MUL, DIV, MOD, JMP, JNZ, JGZ, CPS, SET, SAVE, LOAD, READ, WRITE, PAGE
    };

    enum class TypeModifier {
        C = 0, W, SW, R
    };

    enum class ArgModifier {
        NONE = 0, REF, GZ, EQ
    };

    enum Type {
        BYTE, WORD, SIGNED_WORD, REAL
    };

    using Byte = unsigned char;
    using DoubleByte = unsigned short;
    using Word = unsigned int;
    using SignedWord = int;
    using Real = float;

    static constexpr size_t ARGUMENT_SIZE = sizeof(Word);
    static constexpr size_t CODE_MEMORY_SIZE = std::numeric_limits<DoubleByte>::max() + 1;
    static constexpr size_t DATA_PAGES = std::numeric_limits<Byte>::max() + 1;
    static constexpr size_t PAGE_SIZE = (std::numeric_limits<Byte>::max() + 1) / ARGUMENT_SIZE;

    using Bytes = Bytes<ARGUMENT_SIZE>;
    using Page = std::array<Bytes, PAGE_SIZE>;
    using ByteCode = std::vector<Byte>;
    using SourceToBytecodeMap = std::unordered_map<unsigned int, DoubleByte>;
    using BytecodeToSourceMap = std::unordered_map<DoubleByte, unsigned int>;

    struct OpCodeProperties {
        std::set<TypeModifier> allowed_type_modifiers;
        std::set<ArgModifier> allowed_arg_modifiers;
        bool is_argument_required = true;
        bool is_argument_available = true;
    };

    const std::unordered_map<OpCode, OpCodeProperties> OPCODE_PROPERTIES = {
        {
            OpCode::NOP,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE},
                false,
                true
            }
        },
        {
            OpCode::ADD,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true
            }
        },
        {
            OpCode::SUB,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true
            }
        },
        {
            OpCode::MUL,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true
            }
        },
        {
            OpCode::DIV,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true
            }
        },
        {
            OpCode::MOD,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true
            }
        },
        {
            OpCode::JMP,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true
            }
        },
        {
            OpCode::JGZ,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true
            }
        },
        {
            OpCode::JNZ,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true
            }
        },
        {
            OpCode::SAVE,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true
            }
        },
        {
            OpCode::LOAD,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE},
                true,
                true
            }
        },
        {
            OpCode::READ,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE},
                false,
                false
            }
        },
        {
            OpCode::WRITE,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE},
                false,
                false
            }
        },
        {
            OpCode::PAGE,
            {
                {TypeModifier::C},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true
            }
        },
        {
            OpCode::SET,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true
            }
        },
        {
            OpCode::CPS,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::GZ, ArgModifier::EQ},
                true,
                true
            }
        }
    };

    inline Byte InstructionByte(OpCode opcode, TypeModifier type_modifier,
                                ArgModifier argument_modifier = ArgModifier::NONE) {
        return static_cast<uint8_t>(static_cast<uint8_t>(opcode) << 4) |
            static_cast<uint8_t>(static_cast<uint8_t>(type_modifier) << 2) |
            static_cast<uint8_t>(argument_modifier);
    }
}

#endif
