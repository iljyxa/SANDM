#include "core/assembler.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../core/include/core/common_definitions.hpp"

struct TestParams {
    snm::OpCode opcode;
    snm::ArgModifier arg_modifier;
    snm::TypeModifier type_modifier;
};

const std::vector<TestParams> TEST_COMBINATIONS = {
    // NOPE combinations
    {snm::OpCode::NOPE, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::NOPE, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::NOPE, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::NOPE, snm::ArgModifier::NONE, snm::TypeModifier::R},

    // ADD combinations
    {snm::OpCode::ADD, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::ADD, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::ADD, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::ADD, snm::ArgModifier::NONE, snm::TypeModifier::R},
    {snm::OpCode::ADD, snm::ArgModifier::REF, snm::TypeModifier::C},
    {snm::OpCode::ADD, snm::ArgModifier::REF, snm::TypeModifier::W},
    {snm::OpCode::ADD, snm::ArgModifier::REF, snm::TypeModifier::SW},
    {snm::OpCode::ADD, snm::ArgModifier::REF, snm::TypeModifier::R},
    {snm::OpCode::ADD, snm::ArgModifier::REF_REF, snm::TypeModifier::C},
    {snm::OpCode::ADD, snm::ArgModifier::REF_REF, snm::TypeModifier::W},
    {snm::OpCode::ADD, snm::ArgModifier::REF_REF, snm::TypeModifier::SW},
    {snm::OpCode::ADD, snm::ArgModifier::REF_REF, snm::TypeModifier::R},

    // SUB combinations
    {snm::OpCode::SUB, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::SUB, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::SUB, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::SUB, snm::ArgModifier::NONE, snm::TypeModifier::R},
    {snm::OpCode::SUB, snm::ArgModifier::REF, snm::TypeModifier::C},
    {snm::OpCode::SUB, snm::ArgModifier::REF, snm::TypeModifier::W},
    {snm::OpCode::SUB, snm::ArgModifier::REF, snm::TypeModifier::SW},
    {snm::OpCode::SUB, snm::ArgModifier::REF, snm::TypeModifier::R},
    {snm::OpCode::SUB, snm::ArgModifier::REF_REF, snm::TypeModifier::C},
    {snm::OpCode::SUB, snm::ArgModifier::REF_REF, snm::TypeModifier::W},
    {snm::OpCode::SUB, snm::ArgModifier::REF_REF, snm::TypeModifier::SW},
    {snm::OpCode::SUB, snm::ArgModifier::REF_REF, snm::TypeModifier::R},

    // MUL combinations
    {snm::OpCode::MUL, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::MUL, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::MUL, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::MUL, snm::ArgModifier::NONE, snm::TypeModifier::R},
    {snm::OpCode::MUL, snm::ArgModifier::REF, snm::TypeModifier::C},
    {snm::OpCode::MUL, snm::ArgModifier::REF, snm::TypeModifier::W},
    {snm::OpCode::MUL, snm::ArgModifier::REF, snm::TypeModifier::SW},
    {snm::OpCode::MUL, snm::ArgModifier::REF, snm::TypeModifier::R},
    {snm::OpCode::MUL, snm::ArgModifier::REF_REF, snm::TypeModifier::C},
    {snm::OpCode::MUL, snm::ArgModifier::REF_REF, snm::TypeModifier::W},
    {snm::OpCode::MUL, snm::ArgModifier::REF_REF, snm::TypeModifier::SW},
    {snm::OpCode::MUL, snm::ArgModifier::REF_REF, snm::TypeModifier::R},

    // DIV combinations
    {snm::OpCode::DIV, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::DIV, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::DIV, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::DIV, snm::ArgModifier::NONE, snm::TypeModifier::R},
    {snm::OpCode::DIV, snm::ArgModifier::REF, snm::TypeModifier::C},
    {snm::OpCode::DIV, snm::ArgModifier::REF, snm::TypeModifier::W},
    {snm::OpCode::DIV, snm::ArgModifier::REF, snm::TypeModifier::SW},
    {snm::OpCode::DIV, snm::ArgModifier::REF, snm::TypeModifier::R},
    {snm::OpCode::DIV, snm::ArgModifier::REF_REF, snm::TypeModifier::C},
    {snm::OpCode::DIV, snm::ArgModifier::REF_REF, snm::TypeModifier::W},
    {snm::OpCode::DIV, snm::ArgModifier::REF_REF, snm::TypeModifier::SW},
    {snm::OpCode::DIV, snm::ArgModifier::REF_REF, snm::TypeModifier::R},

    // MOD combinations
    {snm::OpCode::MOD, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::MOD, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::MOD, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::MOD, snm::ArgModifier::REF, snm::TypeModifier::C},
    {snm::OpCode::MOD, snm::ArgModifier::REF, snm::TypeModifier::W},
    {snm::OpCode::MOD, snm::ArgModifier::REF, snm::TypeModifier::SW},
    {snm::OpCode::MOD, snm::ArgModifier::REF_REF, snm::TypeModifier::C},
    {snm::OpCode::MOD, snm::ArgModifier::REF_REF, snm::TypeModifier::W},
    {snm::OpCode::MOD, snm::ArgModifier::REF_REF, snm::TypeModifier::SW},

    // LOAD combinations
    {snm::OpCode::LOAD, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::LOAD, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::LOAD, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::LOAD, snm::ArgModifier::NONE, snm::TypeModifier::R},
    {snm::OpCode::LOAD, snm::ArgModifier::REF, snm::TypeModifier::C},
    {snm::OpCode::LOAD, snm::ArgModifier::REF, snm::TypeModifier::W},
    {snm::OpCode::LOAD, snm::ArgModifier::REF, snm::TypeModifier::SW},
    {snm::OpCode::LOAD, snm::ArgModifier::REF, snm::TypeModifier::R},
    {snm::OpCode::LOAD, snm::ArgModifier::REF_REF, snm::TypeModifier::C},
    {snm::OpCode::LOAD, snm::ArgModifier::REF_REF, snm::TypeModifier::W},
    {snm::OpCode::LOAD, snm::ArgModifier::REF_REF, snm::TypeModifier::SW},
    {snm::OpCode::LOAD, snm::ArgModifier::REF_REF, snm::TypeModifier::R},

    // STORE combinations
    {snm::OpCode::STORE, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::STORE, snm::ArgModifier::REF, snm::TypeModifier::W},

    // INPUT combinations
    {snm::OpCode::INPUT, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::INPUT, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::INPUT, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::INPUT, snm::ArgModifier::NONE, snm::TypeModifier::R},

    // OUTPUT combinations
    {snm::OpCode::OUTPUT, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::OUTPUT, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::OUTPUT, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::OUTPUT, snm::ArgModifier::NONE, snm::TypeModifier::R},

    // JUMP combinations
    {snm::OpCode::JUMP, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::JUMP, snm::ArgModifier::REF, snm::TypeModifier::W},
    {snm::OpCode::JUMP, snm::ArgModifier::REF_REF, snm::TypeModifier::W},

    // SKIP_LOWER combinations
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::NONE, snm::TypeModifier::R},
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::REF, snm::TypeModifier::C},
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::REF, snm::TypeModifier::W},
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::REF, snm::TypeModifier::SW},
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::REF, snm::TypeModifier::R},
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::REF_REF, snm::TypeModifier::C},
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::REF_REF, snm::TypeModifier::W},
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::REF_REF, snm::TypeModifier::SW},
    {snm::OpCode::SKIP_LOWER, snm::ArgModifier::REF_REF, snm::TypeModifier::R},

    // SKIP_GREATER combinations
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::NONE, snm::TypeModifier::R},
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::REF, snm::TypeModifier::C},
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::REF, snm::TypeModifier::W},
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::REF, snm::TypeModifier::SW},
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::REF, snm::TypeModifier::R},
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::REF_REF, snm::TypeModifier::C},
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::REF_REF, snm::TypeModifier::W},
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::REF_REF, snm::TypeModifier::SW},
    {snm::OpCode::SKIP_GREATER, snm::ArgModifier::REF_REF, snm::TypeModifier::R},

    // SKIP_EQUAL combinations
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::NONE, snm::TypeModifier::C},
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::NONE, snm::TypeModifier::SW},
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::NONE, snm::TypeModifier::R},
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::REF, snm::TypeModifier::C},
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::REF, snm::TypeModifier::W},
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::REF, snm::TypeModifier::SW},
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::REF, snm::TypeModifier::R},
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::REF_REF, snm::TypeModifier::C},
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::REF_REF, snm::TypeModifier::W},
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::REF_REF, snm::TypeModifier::SW},
    {snm::OpCode::SKIP_EQUAL, snm::ArgModifier::REF_REF, snm::TypeModifier::R},

    // JUMPNSTORE combinations
    {snm::OpCode::JUMPNSTORE, snm::ArgModifier::NONE, snm::TypeModifier::W},
    {snm::OpCode::JUMPNSTORE, snm::ArgModifier::REF, snm::TypeModifier::W},
    {snm::OpCode::JUMPNSTORE, snm::ArgModifier::REF_REF, snm::TypeModifier::W}
};

class ParamAssemblerTest : public testing::Test, public testing::WithParamInterface<TestParams> {
public:
    [[nodiscard]] snm::ByteCode Compile(const std::string& source) const {
        return assembler_->Compile(source);
    }

    void SetUp() override {
        assembler_ = std::make_unique<Assembler>();
    }
    void TearDown() override {

    }
protected:
    std::unique_ptr<Assembler> assembler_;
};

class AssemblerTest : public testing::Test {
public:
    [[nodiscard]] snm::ByteCode Compile(const std::string& source) const {
        return assembler_->Compile(source);
    }

    void SetUp() override {
        assembler_ = std::make_unique<Assembler>();
    }
    void TearDown() override {

    }
protected:
    std::unique_ptr<Assembler> assembler_;
};

std::string OpCodeToString(const snm::OpCode opcode) {
    switch (opcode) {
    case snm::OpCode::ADD:
        return "Add";
    case snm::OpCode::SUB:
        return "Sub";
    case snm::OpCode::MUL:
        return "Mul";
    case snm::OpCode::DIV:
        return "Div";
    case snm::OpCode::MOD:
        return "Mod";
    case snm::OpCode::LOAD:
        return "Load";
    case snm::OpCode::STORE:
        return "Store";
    case snm::OpCode::INPUT:
        return "Input";
    case snm::OpCode::OUTPUT:
        return "Output";
    case snm::OpCode::JUMP:
        return "Jump";
    case snm::OpCode::SKIP_LOWER:
        return "SkipLo";
    case snm::OpCode::SKIP_GREATER:
        return "SkipGt";
    case snm::OpCode::SKIP_EQUAL:
        return "SkipEq";
    case snm::OpCode::JUMPNSTORE:
        return "JnS";
    case snm::OpCode::NOPE:
        return "Nope";

    default:
        return "Unknown";
    }
}

std::string ArgModifierToString(const snm::ArgModifier mod) {
    switch (mod) {
    case snm::ArgModifier::NONE:
        return "None";
    case snm::ArgModifier::REF:
        return "REF";
    case snm::ArgModifier::REF_REF:
        return "REF_REF";
    default:
        return "Unknown";
    }
}

std::string ArgModifierToStringSource(const snm::ArgModifier mod) {
    switch (mod) {
    case snm::ArgModifier::NONE:
        return "";
    case snm::ArgModifier::REF:
        return "&";
    case snm::ArgModifier::REF_REF:
        return "&&";
    default:
        return "Unknown";
    }
}

std::string TypeModifierToString(const snm::TypeModifier mod) {
    switch (mod) {
    case snm::TypeModifier::C:
        return "C";
    case snm::TypeModifier::W:
        return "W";
    case snm::TypeModifier::SW:
        return "SW";
    case snm::TypeModifier::R:
        return "R";
    default:
        return "Unknown";
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCombinationsCommand,
    ParamAssemblerTest,
    ::testing::ValuesIn(TEST_COMBINATIONS),
    [](const testing::TestParamInfo<TestParams>& info) {
        return std::string(OpCodeToString(info.param.opcode)) + "_" +
               ArgModifierToString(info.param.arg_modifier) + "_" +
               TypeModifierToString(info.param.type_modifier);
    }
);

TEST_F(AssemblerTest, EmptySource) {
    const auto bytecode = Compile("");
    EXPECT_EQ(bytecode.size(), 0);
}

TEST_F(AssemblerTest, CompileSimpleProgram) {
    const std::string source = R"(
        string_hello_world: 'H'
        'e'
        'l'
        'l'
        'o'
        ' '
        'W'
        'o'
        'r'
        'l'
        'd'
        '!'
        0x00

        Load string_hello_world
        JnS Print
        Jump end

        // Функция вывода ascii-строки
        // В регистре должен хранится адрес первого символа
        // Вывод происходит до первого 0 (си-строка)
        // Вывод завершается символами CRLF
        Print:
	        Print_STR: 0 // Адрес текущего символа
	        Print_ACC_original: 0 // Для восстановления значения регистра
	        Store Print_str
	        Store Print_ACC_original

	        Print_Loop:
		        Load && Print_str // Загрузка символа в регистр
		        SkipGt C 0	        // Проверка, что 0 не достигнут
		        Jump Print_End    // Если достигнут, то переход к завершению вызова
		        Output C          // Иначе вывод символа

		        // Увеличение адреса текущего символа на 1
		        Load & Print_str
		        Add 1
		        Store Print_str

		        Jump Print_Loop

	        Print_End:
	        // Вывод символов CRLF
	        Load 0x0D
	        Output C
	        Load 0x0A
	        Output C

	        Load Print_ACC_original // Восстановление значения регистра на момент вызова
	        Jump & Print            // Возврат из функции

        end:
    )";

    const snm::ByteCode result = Compile(source);

    ASSERT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 190);
}

TEST_P(ParamAssemblerTest, TestSourceCodeToByteCode) {
    const auto [opcode, arg_modifier, type_modifier] = GetParam();
    std::string instruction;
    if (snm::OPCODE_PROPERTIES.at(opcode).is_argument_available) {
        instruction = std::format("{} {} {} {}", OpCodeToString(opcode), TypeModifierToString(type_modifier),
                                  ArgModifierToStringSource(arg_modifier), 123);
    } else {
        instruction = std::format("{} {} {}", OpCodeToString(opcode), ArgModifierToStringSource(arg_modifier),
                                  TypeModifierToString(type_modifier));
    }

    const snm::Byte target_byte_code = snm::InstructionByte(opcode, type_modifier, arg_modifier);
    const snm::ByteCode result_byte_code = Compile(instruction);
    ASSERT_EQ(result_byte_code.size(), 5);
    EXPECT_EQ(result_byte_code[0], target_byte_code);
}