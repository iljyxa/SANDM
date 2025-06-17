#ifndef COMMON_DEFINITIONS_HPP
#define COMMON_DEFINITIONS_HPP

#include <limits>
#include <set>
#include <unordered_map>
#include <vector>

#include "core/bytes.hpp"

namespace snm {
    /**
     * @enum OpCode
     * @brief Перечисление кодов операций (опкодов) для виртуального процессора.
     *
     * Определяет набор команд, которые процессор может выполнять.
     */
    enum class OpCode {
        NOPE = 0,
        ADD, SUB, MUL, DIV, MOD,
        LOAD, STORE,
        INPUT, OUTPUT,
        JUMP, JUMPNSTORE, SKIP_LOWER, SKIP_GREATER, SKIP_EQUAL,
        HALT
    };

    /**
     * @enum TypeModifier
     * @brief Перечисление модификаторов типов для настройки параметров.
     *
     * Представляет различные варианты модификаторов, используемые для определения типов аргументов команд процессора
     */
    enum class TypeModifier {
        C = 0, W, SW, R
    };

    /**
     * @enum ArgModifier
     * @brief Перечисление модификаторов аргументов команд процессора.
     *
     * Определяет различные виды ссылочных модификаторов, которые могут быть применены к аргументам команд,
     * включая отсутствие модификации, обычную ссылку и двойную ссылку (указатель).
     */
    enum class ArgModifier {
        NONE = 0, REF, REF_REF
    };

    /**
     * @enum Type
     * @brief Перечисление типов данных для работы процессора.
     *
     * Определяет возможные типы данных, обрабатываемые процессором, включая байты, слова,
     * знаковые слова и числа с плавающей запятой.
     */
    enum Type {
        BYTE, WORD, SIGNED_WORD, REAL
    };

    /**
     * @enum ProcessorState
     * @brief Перечисление состояний процессора.
     */
    enum ProcessorState {
        STOPPED, ///< Остановлен. Процессор не был запущен или был остановлен.
        RUNNING, ///< Выполняется.
        PAUSED_BY_IO, ///< Приостановлен. Процессор работает, но не может выполняться ни одна инструкция, ожидает завершения ввода извне.
        PAUSED ///< Приостановлен
    };

    using Byte = unsigned char;
    using DoubleByte = unsigned short;
    using Word = unsigned int;
    using SignedWord = int;
    using Real = float;
    using Address = DoubleByte;

    static constexpr size_t ARGUMENT_SIZE = sizeof(Word);
    static constexpr size_t CODE_MEMORY_SIZE = std::numeric_limits<Address>::max() + 1;

    using Bytes = Bytes<ARGUMENT_SIZE>;
    using ByteCode = std::vector<Byte>;
    using SourceToBytecodeMap = std::unordered_map<unsigned int, Address>;
    using BytecodeToSourceMap = std::unordered_map<Address, unsigned int>;

    /**
     * @struct OpCodeProperties
     * @brief Структура, задающая свойства операций (опкодов) для виртуального процессора.
     *
     * Описывает параметры и ограничения для каждого опкода. Содержит информацию о допустимых
     * модификаторах типов и аргументов, необходимости наличия аргумента, доступности аргумента,
     * а также строковое имя, соответствующее данному опкоду.
     *
     * @param allowed_type_modifiers Допустимые модификаторы типов аргумента.
     * @param allowed_arg_modifiers Допустимые модификаторы аргументов.
     * @param is_argument_required Указывает, требуется ли аргумент для опкода.
     * @param is_argument_available Указывает, доступен ли аргумент для обработки.
     * @param name Строковое имя, представляющее данный опкод.
     */
    struct OpCodeProperties {
        std::set<TypeModifier> allowed_type_modifiers;
        std::set<ArgModifier> allowed_arg_modifiers;
        bool is_argument_required = true;
        bool is_argument_available = true;
        std::string name;
    };

    /**
     * @var OPCODE_PROPERTIES
     * @brief Словарь, связывающий коды операций (OpCode) с их свойствами (OpCodeProperties).
     *
     * Содержит информацию о модификаторах типов и аргументов, разрешении чтения и записи,
     * а также строковом представлении для каждого опкода.
     * @see OpCodeProperties
     * @see OpCode
     */
    const std::unordered_map<OpCode, OpCodeProperties> OPCODE_PROPERTIES = {
        {
            OpCode::NOPE,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE},
                false,
                true,
                "NOPE"
            }
        },
        {
            OpCode::ADD,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::REF_REF},
                true,
                true,
                "ADD"
            }
        },
        {
            OpCode::SUB,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::REF_REF},
                true,
                true,
                "SUB"
            }
        },
        {
            OpCode::MUL,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::REF_REF},
                true,
                true,
                "MUL"
            }
        },
        {
            OpCode::DIV,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::REF_REF},
                true,
                true,
                "DIV"
            }
        },
        {
            OpCode::MOD,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::REF_REF},
                true,
                true,
                "MOD"
            }
        },
        {
            OpCode::LOAD,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::REF_REF},
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
                {ArgModifier::NONE},
                false,
                false,
                "INPUT"
            }
        },
        {
            OpCode::OUTPUT,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE},
                false,
                false,
                "OUTPUT"
            }
        },
        {
            OpCode::JUMP,
            {
                {TypeModifier::W},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::REF_REF},
                true,
                true,
                "JUMP"
            }
        },
        {
            OpCode::SKIP_LOWER,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::REF_REF},
                true,
                true,
                "SKIPLO"
            }
        },
        {
            OpCode::SKIP_GREATER,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::REF_REF},
                true,
                true,
                "SKIPGT"
            }
        },
        {
            OpCode::SKIP_EQUAL,
            {
                {TypeModifier::C, TypeModifier::W, TypeModifier::SW, TypeModifier::R},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::REF_REF},
                true,
                true,
                "SKIPEQ"
            }
        },
        {
            OpCode::JUMPNSTORE,
            {
                {TypeModifier::W},
                {ArgModifier::NONE, ArgModifier::REF, ArgModifier::REF_REF},
                true,
                true,
                "JNS"
            }
        },
        {
            OpCode::HALT,
            {
                    { },
                    {ArgModifier::NONE},
                    false,
                    false,
                    "HALT"
                }
        }
    };

    /**
     * @brief Создает 8-битное представление инструкции процессора на основе опкода, модификатора типа и модификатора аргумента.
     *
     * Формирует 8-битный код инструкции, объединяя переданные значения согласно их битовому смещению.
     * Первые 4 бита представляют код операции (опкод), следующие 2 бита модификатор типа и последние 2 бита подификатор аргумента.
     * Исключением является HALT - константное значение 11111111
     *
     * @param opcode Код операции (опкод), задающий основную команду процессора.
     * @param type_modifier Модификатор типа, определяющий особенности обработки данных.
     * @param argument_modifier (опционально) Модификатор аргумента, уточняющий дополнительные параметры аргумента.
     *                         По умолчанию равен ArgModifier::NONE.
     *
     * @return 8-битное представление инструкции в формате Byte (unsigned char).
     */
    inline Byte InstructionByte(OpCode opcode, TypeModifier type_modifier, ArgModifier argument_modifier = ArgModifier::NONE) {
        if (opcode == OpCode::HALT) {
            return std::numeric_limits<Byte>::max();
        }

        return static_cast<uint8_t>(static_cast<uint8_t>(opcode) << 4) |
               static_cast<uint8_t>(static_cast<uint8_t>(type_modifier) << 2) |
               static_cast<uint8_t>(argument_modifier);
    }
}

#endif
