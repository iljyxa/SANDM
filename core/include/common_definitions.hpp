#ifndef COMMON_DEFINITIONS_HPP
#define COMMON_DEFINITIONS_HPP

#include <limits>
#include <set>
#include <vector>
#include "bytes.hpp"

namespace common {
    enum class OpCode {
        NOPE = 0, ADD, SUB, MUL, DIV, MOD, AND, NOT, LOAD, STORE, INPUT, OUTPUT, JUMP, JUMPNSTORE, SKIP_LOWER, SKIP_GREATER, SKIP_EQUAL
    };

    enum class TypeModifier {
        C = 0, W, SW, R
    };

    enum class ArgModifier {
        NONE = 0, REF
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

    using Bytes = Bytes<ARGUMENT_SIZE>;
    using ByteCode = std::vector<Byte>;
    using SourceToBytecodeMap = std::unordered_map<unsigned int, DoubleByte>;
    using BytecodeToSourceMap = std::unordered_map<DoubleByte, unsigned int>;

    struct OpCodeProperties {
        std::set<TypeModifier> allowed_type_modifiers;
        std::set<ArgModifier> allowed_arg_modifiers;
        bool is_argument_required = true;
        bool is_argument_available = true;
        std::string name;
    };

    const std::unordered_map<OpCode, OpCodeProperties> OPCODE_PROPERTIES = {
        {
            OpCode::NOPE,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::REF},
                false,
                false,
                "NOPE"
            }
        },
        {
            OpCode::ADD,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true,
                "ADD"
            }
        },
        {
            OpCode::SUB,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true,
                "SUB"
            }
        },
        {
            OpCode::MUL,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true,
                "MUL"
            }
        },
        {
            OpCode::DIV,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true,
                "DIV"
            }
        },
        {
            OpCode::MOD,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true,
                "MOD"
            }
        },
        {
            OpCode::LOAD,
            {
                {TypeModifier::W},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true,
                "LOAD"
            }
        },
        {
            OpCode::STORE,
            {
                {TypeModifier::W},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true,
                "STORE"
            }
        },
        {
            OpCode::INPUT,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                { },
                false,
                false,
                "INPUT"
            }
        },
        {
            OpCode::OUTPUT,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                { },
                false,
                false,
                "OUTPUT"
            }
        },
        {
            OpCode::JUMP,
            {
                {TypeModifier::W},
                {ArgModifier::NONE},
                true,
                true,
                "JUMP"
            }
        },
        {
            OpCode::SKIP_LOWER,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true,
                "SKIPLO"
            }
        },
        {
            OpCode::SKIP_GREATER,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true,
                "SKIPGT"
            }
        },
        {
            OpCode::SKIP_EQUAL,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF},
                true,
                true,
                "SKIPEQ"
            }
        },
        {
            OpCode::JUMPNSTORE,
            {
                {TypeModifier::W},
                {ArgModifier::NONE},
                true,
                true,
                "JNS"
            }
        }
    };

    inline Byte InstructionByte(OpCode opcode, TypeModifier type_modifier, ArgModifier argument_modifier = ArgModifier::NONE) {
        return static_cast<uint8_t>(static_cast<uint8_t>(opcode) << 3) |
               static_cast<uint8_t>(static_cast<uint8_t>(type_modifier) << 1) |
               static_cast<uint8_t>(argument_modifier);
    }
}

#endif
