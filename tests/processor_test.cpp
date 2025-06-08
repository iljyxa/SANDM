#include <gtest/gtest.h>

#include "core/processor.hpp"
#include "core/assembler.hpp"

class ProcessorTest : public testing::Test, public testing::WithParamInterface<snm::ArgModifier> {
public:
    std::unique_ptr<MemoryManager> memory;
    std::unique_ptr<Processor> processor;

    void SetA(const snm::Bytes value) const {
        processor->SetAccumulator(value);
    }

    void SetB(const snm::Bytes value) {
        argument_ = value;
    }

    void WriteInstruction(const snm::OpCode opcode, const snm::TypeModifier type_modifier,
                          const snm::ArgModifier arg_modifier, const snm::Bytes argument,
                          const snm::Address address) const {
        memory->WriteInstruction(snm::InstructionByte(opcode, type_modifier, arg_modifier), argument, address);
    }

    void Step() const {
        processor->Step();
    }

    [[nodiscard]] snm::Bytes Calc(const snm::OpCode opcode, const snm::TypeModifier type_modifier,
                                  const snm::ArgModifier arg_modifier) const {
        if (arg_modifier == snm::ArgModifier::REF) {
            constexpr snm::Address address = 256;
            memory->WriteInstruction(snm::InstructionByte(opcode, type_modifier, arg_modifier), snm::Bytes(address));
            memory->WriteArgument(argument_, address);
        } else if (arg_modifier == snm::ArgModifier::REF_REF) {
            constexpr snm::Address address_pointer = 9000;
            constexpr snm::Address address_value = 10000;

            memory->WriteInstruction(snm::InstructionByte(opcode, type_modifier, arg_modifier), snm::Bytes(address_pointer));
            memory->WriteArgument(argument_, address_value);
            memory->WriteArgument(snm::Bytes(address_value), address_pointer);
        } else {
            memory->WriteInstruction(snm::InstructionByte(opcode, type_modifier, arg_modifier), argument_);
        }

        processor->Step();

        const snm::Bytes result = processor->GetAccumulator();

        Reset();

        return result;
    }

    void Exec(const snm::OpCode opcode, const snm::TypeModifier type_modifier,
              const snm::ArgModifier arg_modifier) const {
        if (arg_modifier == snm::ArgModifier::REF) {
            constexpr snm::Address address = 256;
            memory->WriteInstruction(snm::InstructionByte(opcode, type_modifier, arg_modifier), snm::Bytes(address));
            memory->WriteArgument(argument_, address);
        } else if (arg_modifier == snm::ArgModifier::REF_REF) {
            constexpr snm::Address address_pointer = 9000;
            constexpr snm::Address address_value = 10000;

            memory->WriteInstruction(snm::InstructionByte(opcode, type_modifier, arg_modifier), snm::Bytes(address_pointer));
            memory->WriteArgument(argument_, address_value);
            memory->WriteArgument(snm::Bytes(address_value), address_pointer);
        } else {
            memory->WriteInstruction(snm::InstructionByte(opcode, type_modifier, arg_modifier), argument_);
        }

        processor->Step();
    }

    [[nodiscard]] snm::Bytes ReadMemory(const snm::Address address) const {
        return memory->ReadArgument(address);
    }

    // ReSharper disable once CppInconsistentNaming
    [[nodiscard]] snm::Address GetIP() const {
        return processor->GetInstructionPointer();
    }

    void Reset() const {
        memory->Reset();
        processor->Reset();
    }

    void SetUp() override {
        memory = std::make_unique<MemoryManager>();
        processor = std::make_unique<Processor>(*memory);
    }
    void TearDown() override {
        Reset();
    }
private:
    snm::Bytes argument_ = {};
};

INSTANTIATE_TEST_SUITE_P(
    Processor,
    ProcessorTest,
    ::testing::Values(
        snm::ArgModifier::NONE,
        snm::ArgModifier::REF,
        snm::ArgModifier::REF_REF
    ),
    [](const testing::TestParamInfo<snm::ArgModifier>& info) {
    switch (info.param) {
    case snm::ArgModifier::NONE: return "VALUE";
    case snm::ArgModifier::REF: return "REF";
    case snm::ArgModifier::REF_REF: return "REF_REF";
            default: return "Unknown";
        }
    }
);

TEST_F(ProcessorTest, ExecuteSimpleProgram) {
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

    Assembler assembler{};
    const snm::ByteCode byte_code = assembler.Compile(source);
    memory->Load(byte_code);
    ASSERT_NO_THROW(processor->Run());
}

TEST_P(ProcessorTest, Add) {
    constexpr auto opcode = snm::OpCode::ADD;
    const auto arg_modifier = GetParam();
    snm::Bytes result{};

    SetA(snm::Bytes(128));
    SetB(snm::Bytes(127));
    result = Calc(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<snm::Byte>(result), 255);

    SetA(snm::Bytes(1000000000));
    SetB(snm::Bytes(1034567892));
    result = Calc(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<snm::Word>(result), 2034567892);

    SetA(snm::Bytes(std::numeric_limits<snm::SignedWord>::min()));
    SetB(snm::Bytes(std::numeric_limits<snm::SignedWord>::max()));
    result = Calc(opcode, snm::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(static_cast<snm::SignedWord>(result), -1);

    SetA(snm::Bytes(static_cast<snm::Real>(1 / 3.0)));
    SetB(snm::Bytes(static_cast<snm::Real>(2 / 3.0)));
    result = Calc(opcode, snm::TypeModifier::R, arg_modifier);
    EXPECT_EQ(static_cast<snm::Real>(result), 1);
}

TEST_P(ProcessorTest, Sub) {
    constexpr auto opcode = snm::OpCode::SUB;
    const auto arg_modifier = GetParam();
    snm::Bytes result{};

    SetA(snm::Bytes(128));
    SetB(snm::Bytes(127));
    result = Calc(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<snm::Byte>(result), 1);

    SetA(snm::Bytes(1034567892));
    SetB(snm::Bytes(1000000000));
    result = Calc(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<snm::Word>(result), 34567892);

    SetA(snm::Bytes(std::numeric_limits<snm::SignedWord>::min()));
    SetB(snm::Bytes(-std::numeric_limits<snm::SignedWord>::max()));
    result = Calc(opcode, snm::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(static_cast<snm::SignedWord>(result), -1);

    SetA(snm::Bytes(static_cast<snm::Real>(2 / 3.0)));
    SetB(snm::Bytes(static_cast<snm::Real>(1 / 3.0)));
    result = Calc(opcode, snm::TypeModifier::R, arg_modifier);
    EXPECT_EQ(static_cast<snm::Real>(result), static_cast<snm::Real>(2 / 3.0) - static_cast<snm::Real>(1 / 3.0));
}

TEST_P(ProcessorTest, Mul) {
    constexpr auto opcode = snm::OpCode::MUL;
    const auto arg_modifier = GetParam();
    snm::Bytes result{};

    SetA(snm::Bytes(5));
    SetB(snm::Bytes(6));
    result = Calc(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<snm::Byte>(result), static_cast<snm::Byte>(5) * static_cast<snm::Byte>(6));

    SetA(snm::Bytes(123456));
    SetB(snm::Bytes(789012));
    result = Calc(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<snm::Word>(result), static_cast<snm::Word>(123456) * static_cast<snm::Word>(789012));

    SetA(snm::Bytes(-12345));
    SetB(snm::Bytes(78901));
    result = Calc(opcode, snm::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(static_cast<snm::SignedWord>(result), -12345 * 78901);

    SetA(snm::Bytes(static_cast<snm::Real>(2 / 3.0)));
    SetB(snm::Bytes(static_cast<snm::Real>(1 / 3.0)));
    result = Calc(opcode, snm::TypeModifier::R, arg_modifier);
    EXPECT_EQ(static_cast<snm::Real>(result), static_cast<snm::Real>(2 / 3.0) * static_cast<snm::Real>(1 / 3.0));
}

TEST_P(ProcessorTest, Div) {
    constexpr auto opcode = snm::OpCode::DIV;
    const auto arg_modifier = GetParam();
    snm::Bytes result{};

    SetA(snm::Bytes(6));
    SetB(snm::Bytes(5));
    result = Calc(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<snm::Byte>(result), static_cast<snm::Byte>(6) / static_cast<snm::Byte>(5));

    SetA(snm::Bytes(789012));
    SetB(snm::Bytes(123456));
    result = Calc(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<snm::Word>(result), static_cast<snm::Word>(789012) / static_cast<snm::Word>(123456));

    SetA(snm::Bytes(78901));
    SetB(snm::Bytes(-12345));
    result = Calc(opcode, snm::TypeModifier::SW, arg_modifier);
    // ReSharper disable once CppRedundantCastExpression
    EXPECT_EQ(static_cast<snm::SignedWord>(result), static_cast<snm::SignedWord>(78901) / static_cast<snm::SignedWord>(-
                  12345));

    SetA(snm::Bytes(static_cast<snm::Real>(1)));
    SetB(snm::Bytes(static_cast<snm::Real>(3)));
    result = Calc(opcode, snm::TypeModifier::R, arg_modifier);
    EXPECT_EQ(static_cast<snm::Real>(result), static_cast<snm::Real>(1) / static_cast<snm::Real>(3));

    SetA(snm::Bytes(1));
    SetB(snm::Bytes(0));
    // ReSharper disable once CppNoDiscardExpression
    EXPECT_THROW(Calc(opcode, snm::TypeModifier::W, arg_modifier), std::runtime_error);
}

TEST_P(ProcessorTest, Mod) {
    constexpr auto opcode = snm::OpCode::MOD;
    const auto arg_modifier = GetParam();
    snm::Bytes result{};

    SetA(snm::Bytes(6));
    SetB(snm::Bytes(5));
    result = Calc(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<snm::Byte>(result), static_cast<snm::Byte>(6) % static_cast<snm::Byte>(5));

    SetA(snm::Bytes(789012));
    SetB(snm::Bytes(123456));
    result = Calc(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<snm::Word>(result), static_cast<snm::Word>(789012) % static_cast<snm::Word>(123456));

    SetA(snm::Bytes(78901));
    SetB(snm::Bytes(-12345));
    result = Calc(opcode, snm::TypeModifier::SW, arg_modifier);
    // ReSharper disable once CppRedundantCastExpression
    EXPECT_EQ(static_cast<snm::SignedWord>(result), static_cast<snm::SignedWord>(78901) % static_cast<snm::SignedWord>(-
                  12345));

    SetA(snm::Bytes(static_cast<snm::Real>(1)));
    SetB(snm::Bytes(static_cast<snm::Real>(3)));
    // ReSharper disable once CppNoDiscardExpression
    EXPECT_THROW(Calc(opcode, snm::TypeModifier::R, arg_modifier), std::runtime_error);

    SetA(snm::Bytes(1));
    SetB(snm::Bytes(0));
    // ReSharper disable once CppNoDiscardExpression
    EXPECT_THROW(Calc(opcode, snm::TypeModifier::W, arg_modifier), std::runtime_error);
}

TEST_P(ProcessorTest, Load) {
    constexpr auto opcode = snm::OpCode::LOAD;
    const auto arg_modifier = GetParam();
    snm::Bytes result{};

    SetB(snm::Bytes(255));
    result = Calc(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<snm::Byte>(result), 255);

    SetB(snm::Bytes(std::numeric_limits<snm::Word>::max()));
    result = Calc(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<snm::Word>(result), std::numeric_limits<snm::Word>::max());

    SetB(snm::Bytes(std::numeric_limits<snm::SignedWord>::min()));
    result = Calc(opcode, snm::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(static_cast<snm::SignedWord>(result), std::numeric_limits<snm::SignedWord>::min());

    SetB(snm::Bytes(std::numeric_limits<snm::Real>::max()));
    result = Calc(opcode, snm::TypeModifier::R, arg_modifier);
    EXPECT_EQ(static_cast<snm::Real>(result), std::numeric_limits<snm::Real>::max());
}

TEST_P(ProcessorTest, Store) {
    constexpr auto opcode = snm::OpCode::STORE;
    const auto arg_modifier = GetParam();

    if (arg_modifier == snm::ArgModifier::REF_REF) {
        GTEST_SKIP() << "Skipping REF_REF for this case";
    }

    snm::Bytes result{};
    constexpr snm::Address address = 1234;

    SetA(snm::Bytes(std::numeric_limits<snm::Real>::max()));
    SetB(snm::Bytes(address));
    Exec(opcode, snm::TypeModifier::W, arg_modifier);
    result = ReadMemory(address);
    EXPECT_EQ(static_cast<snm::Real>(result), std::numeric_limits<snm::Real>::max());
}

TEST_P(ProcessorTest, Jump) {
    constexpr auto opcode = snm::OpCode::JUMP;
    const auto arg_modifier = GetParam();
    constexpr snm::Address address = 1234;

    SetB(snm::Bytes(address));
    Exec(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), address);
}

TEST_P(ProcessorTest, SkipLo_True) {
    constexpr auto opcode = snm::OpCode::SKIP_LOWER;
    const auto arg_modifier = GetParam();
    constexpr snm::Address target_ip_value = 2;

    SetA(snm::Bytes(1));
    SetB(snm::Bytes(5));
    Exec(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(std::numeric_limits<snm::Word>::max() - 1));
    SetB(snm::Bytes(std::numeric_limits<snm::Word>::max()));
    Exec(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(std::numeric_limits<snm::SignedWord>::min()));
    SetB(snm::Bytes(std::numeric_limits<snm::SignedWord>::min() + 1));
    Exec(opcode, snm::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(static_cast<snm::Real>(3.12)));
    SetB(snm::Bytes(static_cast<snm::Real>(3.14)));
    Exec(opcode, snm::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, SkipLo_False) {
    constexpr auto opcode = snm::OpCode::SKIP_LOWER;
    const auto arg_modifier = GetParam();
    constexpr snm::Address target_ip_value = 1;

    SetA(snm::Bytes(5));
    SetB(snm::Bytes(1));
    Exec(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(std::numeric_limits<snm::Word>::max()));
    SetB(snm::Bytes(std::numeric_limits<snm::Word>::max() - 1));
    Exec(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(std::numeric_limits<snm::SignedWord>::min() + 1));
    SetB(snm::Bytes(std::numeric_limits<snm::SignedWord>::min()));
    Exec(opcode, snm::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(static_cast<snm::Real>(3.14)));
    SetB(snm::Bytes(static_cast<snm::Real>(3.12)));
    Exec(opcode, snm::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, SkipEq_True) {
    constexpr auto opcode = snm::OpCode::SKIP_EQUAL;
    const auto arg_modifier = GetParam();
    constexpr snm::Address target_ip_value = 2;

    SetA(snm::Bytes(5));
    SetB(snm::Bytes(5));
    Exec(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(std::numeric_limits<snm::Word>::max()));
    SetB(snm::Bytes(std::numeric_limits<snm::Word>::max()));
    Exec(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(std::numeric_limits<snm::SignedWord>::min()));
    SetB(snm::Bytes(std::numeric_limits<snm::SignedWord>::min()));
    Exec(opcode, snm::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(static_cast<snm::Real>(3.14)));
    SetB(snm::Bytes(static_cast<snm::Real>(3.14)));
    Exec(opcode, snm::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, SkipEq_False) {
    constexpr auto opcode = snm::OpCode::SKIP_EQUAL;
    const auto arg_modifier = GetParam();
    constexpr snm::Address target_ip_value = 1;

    SetA(snm::Bytes(10));
    SetB(snm::Bytes(5));
    Exec(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(123));
    SetB(snm::Bytes(std::numeric_limits<snm::Word>::max()));
    Exec(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(789));
    SetB(snm::Bytes(std::numeric_limits<snm::SignedWord>::min()));
    Exec(opcode, snm::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(static_cast<snm::Real>(3.15)));
    SetB(snm::Bytes(static_cast<snm::Real>(3.14)));
    Exec(opcode, snm::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, SkipGt_True) {
    constexpr auto opcode = snm::OpCode::SKIP_GREATER;
    const auto arg_modifier = GetParam();
    constexpr snm::Address target_ip_value = 2;

    SetA(snm::Bytes(10));
    SetB(snm::Bytes(5));
    Exec(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(std::numeric_limits<snm::Word>::max()));
    SetB(snm::Bytes(123));
    Exec(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(0));
    SetB(snm::Bytes(std::numeric_limits<snm::SignedWord>::min()));
    Exec(opcode, snm::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(static_cast<snm::Real>(3.15)));
    SetB(snm::Bytes(static_cast<snm::Real>(3.14)));
    Exec(opcode, snm::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, SkipGt_False) {
    constexpr auto opcode = snm::OpCode::SKIP_GREATER;
    const auto arg_modifier = GetParam();
    constexpr snm::Address target_ip_value = 1;

    SetA(snm::Bytes(5));
    SetB(snm::Bytes(10));
    Exec(opcode, snm::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(123));
    SetB(snm::Bytes(std::numeric_limits<snm::Word>::max()));
    Exec(opcode, snm::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(std::numeric_limits<snm::SignedWord>::min()));
    SetB(snm::Bytes(789));
    Exec(opcode, snm::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(snm::Bytes(static_cast<snm::Real>(3.14)));
    SetB(snm::Bytes(static_cast<snm::Real>(3.15)));
    Exec(opcode, snm::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, JumpNStore) {
    constexpr auto opcode = snm::OpCode::JUMPNSTORE;
    const auto arg_modifier = GetParam();
    constexpr snm::Address target_address = 5;

    SetB(snm::Bytes(target_address));
    Exec(opcode, snm::TypeModifier::W, arg_modifier);
    // Проверка, что произошел прыжок
    EXPECT_EQ(GetIP(), target_address + 1);
    // Проверка, что был сохранен возвратный адрес
    EXPECT_EQ(static_cast<snm::Address>(ReadMemory(target_address)), 1);
}