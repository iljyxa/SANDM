#include "core/assembler.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../core/include/core/common_definitions.hpp"

struct TestParams {
    common::OpCode opcode;
    common::ArgModifier arg_modifier;
    common::TypeModifier type_modifier;
};

const std::vector<TestParams> test_combinations = {
    // NOPE combinations
    {common::OpCode::NOPE, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::NOPE, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::NOPE, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::NOPE, common::ArgModifier::NONE, common::TypeModifier::R},

    // ADD combinations
    {common::OpCode::ADD, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::ADD, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::ADD, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::ADD, common::ArgModifier::NONE, common::TypeModifier::R},
    {common::OpCode::ADD, common::ArgModifier::REF, common::TypeModifier::C},
    {common::OpCode::ADD, common::ArgModifier::REF, common::TypeModifier::W},
    {common::OpCode::ADD, common::ArgModifier::REF, common::TypeModifier::SW},
    {common::OpCode::ADD, common::ArgModifier::REF, common::TypeModifier::R},
    {common::OpCode::ADD, common::ArgModifier::REF_REF, common::TypeModifier::C},
    {common::OpCode::ADD, common::ArgModifier::REF_REF, common::TypeModifier::W},
    {common::OpCode::ADD, common::ArgModifier::REF_REF, common::TypeModifier::SW},
    {common::OpCode::ADD, common::ArgModifier::REF_REF, common::TypeModifier::R},

    // SUB combinations 
    {common::OpCode::SUB, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::SUB, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::SUB, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::SUB, common::ArgModifier::NONE, common::TypeModifier::R},
    {common::OpCode::SUB, common::ArgModifier::REF, common::TypeModifier::C},
    {common::OpCode::SUB, common::ArgModifier::REF, common::TypeModifier::W},
    {common::OpCode::SUB, common::ArgModifier::REF, common::TypeModifier::SW},
    {common::OpCode::SUB, common::ArgModifier::REF, common::TypeModifier::R},
    {common::OpCode::SUB, common::ArgModifier::REF_REF, common::TypeModifier::C},
    {common::OpCode::SUB, common::ArgModifier::REF_REF, common::TypeModifier::W},
    {common::OpCode::SUB, common::ArgModifier::REF_REF, common::TypeModifier::SW},
    {common::OpCode::SUB, common::ArgModifier::REF_REF, common::TypeModifier::R},

    // MUL combinations
    {common::OpCode::MUL, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::MUL, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::MUL, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::MUL, common::ArgModifier::NONE, common::TypeModifier::R},
    {common::OpCode::MUL, common::ArgModifier::REF, common::TypeModifier::C},
    {common::OpCode::MUL, common::ArgModifier::REF, common::TypeModifier::W},
    {common::OpCode::MUL, common::ArgModifier::REF, common::TypeModifier::SW},
    {common::OpCode::MUL, common::ArgModifier::REF, common::TypeModifier::R},
    {common::OpCode::MUL, common::ArgModifier::REF_REF, common::TypeModifier::C},
    {common::OpCode::MUL, common::ArgModifier::REF_REF, common::TypeModifier::W},
    {common::OpCode::MUL, common::ArgModifier::REF_REF, common::TypeModifier::SW},
    {common::OpCode::MUL, common::ArgModifier::REF_REF, common::TypeModifier::R},

    // DIV combinations
    {common::OpCode::DIV, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::DIV, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::DIV, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::DIV, common::ArgModifier::NONE, common::TypeModifier::R},
    {common::OpCode::DIV, common::ArgModifier::REF, common::TypeModifier::C},
    {common::OpCode::DIV, common::ArgModifier::REF, common::TypeModifier::W},
    {common::OpCode::DIV, common::ArgModifier::REF, common::TypeModifier::SW},
    {common::OpCode::DIV, common::ArgModifier::REF, common::TypeModifier::R},
    {common::OpCode::DIV, common::ArgModifier::REF_REF, common::TypeModifier::C},
    {common::OpCode::DIV, common::ArgModifier::REF_REF, common::TypeModifier::W},
    {common::OpCode::DIV, common::ArgModifier::REF_REF, common::TypeModifier::SW},
    {common::OpCode::DIV, common::ArgModifier::REF_REF, common::TypeModifier::R},

    // MOD combinations
    {common::OpCode::MOD, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::MOD, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::MOD, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::MOD, common::ArgModifier::REF, common::TypeModifier::C},
    {common::OpCode::MOD, common::ArgModifier::REF, common::TypeModifier::W},
    {common::OpCode::MOD, common::ArgModifier::REF, common::TypeModifier::SW},
    {common::OpCode::MOD, common::ArgModifier::REF_REF, common::TypeModifier::C},
    {common::OpCode::MOD, common::ArgModifier::REF_REF, common::TypeModifier::W},
    {common::OpCode::MOD, common::ArgModifier::REF_REF, common::TypeModifier::SW},

    // LOAD combinations
    {common::OpCode::LOAD, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::LOAD, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::LOAD, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::LOAD, common::ArgModifier::NONE, common::TypeModifier::R},
    {common::OpCode::LOAD, common::ArgModifier::REF, common::TypeModifier::C},
    {common::OpCode::LOAD, common::ArgModifier::REF, common::TypeModifier::W},
    {common::OpCode::LOAD, common::ArgModifier::REF, common::TypeModifier::SW},
    {common::OpCode::LOAD, common::ArgModifier::REF, common::TypeModifier::R},
    {common::OpCode::LOAD, common::ArgModifier::REF_REF, common::TypeModifier::C},
    {common::OpCode::LOAD, common::ArgModifier::REF_REF, common::TypeModifier::W},
    {common::OpCode::LOAD, common::ArgModifier::REF_REF, common::TypeModifier::SW},
    {common::OpCode::LOAD, common::ArgModifier::REF_REF, common::TypeModifier::R},

    // STORE combinations
    {common::OpCode::STORE, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::STORE, common::ArgModifier::REF, common::TypeModifier::W},

    // INPUT combinations
    {common::OpCode::INPUT, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::INPUT, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::INPUT, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::INPUT, common::ArgModifier::NONE, common::TypeModifier::R},

    // OUTPUT combinations
    {common::OpCode::OUTPUT, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::OUTPUT, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::OUTPUT, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::OUTPUT, common::ArgModifier::NONE, common::TypeModifier::R},

    // JUMP combinations
    {common::OpCode::JUMP, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::JUMP, common::ArgModifier::REF, common::TypeModifier::W},
    {common::OpCode::JUMP, common::ArgModifier::REF_REF, common::TypeModifier::W},

    // SKIP_LOWER combinations
    {common::OpCode::SKIP_LOWER, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::SKIP_LOWER, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::SKIP_LOWER, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::SKIP_LOWER, common::ArgModifier::NONE, common::TypeModifier::R},
    {common::OpCode::SKIP_LOWER, common::ArgModifier::REF, common::TypeModifier::C},
    {common::OpCode::SKIP_LOWER, common::ArgModifier::REF, common::TypeModifier::W},
    {common::OpCode::SKIP_LOWER, common::ArgModifier::REF, common::TypeModifier::SW},
    {common::OpCode::SKIP_LOWER, common::ArgModifier::REF, common::TypeModifier::R},
    {common::OpCode::SKIP_LOWER, common::ArgModifier::REF_REF, common::TypeModifier::C},
    {common::OpCode::SKIP_LOWER, common::ArgModifier::REF_REF, common::TypeModifier::W},
    {common::OpCode::SKIP_LOWER, common::ArgModifier::REF_REF, common::TypeModifier::SW},
    {common::OpCode::SKIP_LOWER, common::ArgModifier::REF_REF, common::TypeModifier::R},

    // SKIP_GREATER combinations
    {common::OpCode::SKIP_GREATER, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::SKIP_GREATER, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::SKIP_GREATER, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::SKIP_GREATER, common::ArgModifier::NONE, common::TypeModifier::R},
    {common::OpCode::SKIP_GREATER, common::ArgModifier::REF, common::TypeModifier::C},
    {common::OpCode::SKIP_GREATER, common::ArgModifier::REF, common::TypeModifier::W},
    {common::OpCode::SKIP_GREATER, common::ArgModifier::REF, common::TypeModifier::SW},
    {common::OpCode::SKIP_GREATER, common::ArgModifier::REF, common::TypeModifier::R},
    {common::OpCode::SKIP_GREATER, common::ArgModifier::REF_REF, common::TypeModifier::C},
    {common::OpCode::SKIP_GREATER, common::ArgModifier::REF_REF, common::TypeModifier::W},
    {common::OpCode::SKIP_GREATER, common::ArgModifier::REF_REF, common::TypeModifier::SW},
    {common::OpCode::SKIP_GREATER, common::ArgModifier::REF_REF, common::TypeModifier::R},

    // SKIP_EQUAL combinations
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::NONE, common::TypeModifier::C},
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::NONE, common::TypeModifier::SW},
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::NONE, common::TypeModifier::R},
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::REF, common::TypeModifier::C},
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::REF, common::TypeModifier::W},
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::REF, common::TypeModifier::SW},
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::REF, common::TypeModifier::R},
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::REF_REF, common::TypeModifier::C},
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::REF_REF, common::TypeModifier::W},
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::REF_REF, common::TypeModifier::SW},
    {common::OpCode::SKIP_EQUAL, common::ArgModifier::REF_REF, common::TypeModifier::R},

    // JUMPNSTORE combinations
    {common::OpCode::JUMPNSTORE, common::ArgModifier::NONE, common::TypeModifier::W},
    {common::OpCode::JUMPNSTORE, common::ArgModifier::REF, common::TypeModifier::W},
    {common::OpCode::JUMPNSTORE, common::ArgModifier::REF_REF, common::TypeModifier::W}
};

class ParamAssemblerTest : public ::testing::Test, public ::testing::WithParamInterface<TestParams> {
public:

    common::ByteCode Compile(const std::string& source) {
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

class AssemblerTest : public ::testing::Test {
public:

    common::ByteCode Compile(const std::string& source) {
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

std::string OpCodeToString(common::OpCode opcode) {
    switch (opcode) {
    case common::OpCode::ADD:
        return "Add";
    case common::OpCode::SUB:
        return "Sub";
    case common::OpCode::MUL:
        return "Mul";
    case common::OpCode::DIV:
        return "Div";
    case common::OpCode::MOD:
        return "Mod";
    case common::OpCode::LOAD:
        return "Load";
    case common::OpCode::STORE:
        return "Store";
    case common::OpCode::INPUT:
        return "Input";
    case common::OpCode::OUTPUT:
        return "Output";
    case common::OpCode::JUMP:
        return "Jump";
    case common::OpCode::SKIP_LOWER:
        return "SkipLo";
    case common::OpCode::SKIP_GREATER:
        return "SkipGt";
    case common::OpCode::SKIP_EQUAL:
        return "SkipEq";
    case common::OpCode::JUMPNSTORE:
        return "JnS";
    case common::OpCode::NOPE:
        return "Nope";

    default:
        return "Unknown";
    }
}

std::string ArgModifierToString(common::ArgModifier mod) {
    switch (mod) {
    case common::ArgModifier::NONE:
        return "None";
    case common::ArgModifier::REF:
        return "REF";
    case common::ArgModifier::REF_REF:
        return "REF_REF";
    default:
        return "Unknown";
    }
}

std::string ArgModifierToStringSource(common::ArgModifier mod) {
    switch (mod) {
    case common::ArgModifier::NONE:
        return "";
    case common::ArgModifier::REF:
        return "&";
    case common::ArgModifier::REF_REF:
        return "&&";
    default:
        return "Unknown";
    }
}

std::string TypeModifierToString(common::TypeModifier mod) {
    switch (mod) {
    case common::TypeModifier::C:
        return "C";
    case common::TypeModifier::W:
        return "W";
    case common::TypeModifier::SW:
        return "SW";
    case common::TypeModifier::R:
        return "R";
    default:
        return "Unknown";
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCombinationsCommand,
    ParamAssemblerTest,
    ::testing::ValuesIn(test_combinations),
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
    std::string source = R"(
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

    common::ByteCode result = Compile(source);

    ASSERT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 190);
}

TEST_P(ParamAssemblerTest, TestSourceCodeToByteCode) {
    const auto params = GetParam();
    std::string instruction;
    if (common::OPCODE_PROPERTIES.at(params.opcode).is_argument_available) {
        instruction = std::format("{} {} {} {}", OpCodeToString(params.opcode), TypeModifierToString(params.type_modifier), ArgModifierToStringSource(params.arg_modifier), 123);
    } else {
        instruction = std::format("{} {} {}", OpCodeToString(params.opcode), ArgModifierToStringSource(params.arg_modifier), TypeModifierToString(params.type_modifier));
    }

    common::Byte target_byte_code = common::InstructionByte(params.opcode, params.type_modifier, params.arg_modifier);
    const common::ByteCode result_byte_code = Compile(instruction);
    ASSERT_EQ(result_byte_code.size(), 5);
    EXPECT_EQ(result_byte_code[0], target_byte_code);
}