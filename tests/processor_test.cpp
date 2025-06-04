#include "core/processor.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class ProcessorTest : public ::testing::Test, public ::testing::WithParamInterface<common::ArgModifier> {
public:
    void SetA(const common::Bytes value) const {
        processor_->SetAccumulator(value);
    }

    void SetB(const common::Bytes value) {
        argument_ = value;
    }

    void WriteInstruction(const common::OpCode opcode, const common::TypeModifier type_modifier,
                          const common::ArgModifier arg_modifier, const common::Bytes argument,
                          const common::DoubleByte address) const {
        memory_->WriteInstruction(common::InstructionByte(opcode, type_modifier, arg_modifier), argument, address);
    }

    void Step() const {
        processor_->Step();
    }

    [[nodiscard]] common::Bytes Calc(const common::OpCode opcode, const common::TypeModifier type_modifier, const common::ArgModifier arg_modifier) const {
        if (arg_modifier == common::ArgModifier::REF) {
            constexpr common::DoubleByte address = 256;
            memory_->WriteInstruction(common::InstructionByte(opcode, type_modifier, arg_modifier), common::Bytes(address));
            memory_->WriteArgument(argument_, address);
        } else if (arg_modifier == common::ArgModifier::REF_REF) {
            constexpr common::DoubleByte address_pointer = 9000;
            constexpr common::DoubleByte address_value = 10000;

            memory_->WriteInstruction(common::InstructionByte(opcode, type_modifier, arg_modifier), common::Bytes(address_pointer));
            memory_->WriteArgument(argument_, address_value);
            memory_->WriteArgument(common::Bytes(address_value), address_pointer);
        } else {
            memory_->WriteInstruction(common::InstructionByte(opcode, type_modifier, arg_modifier), argument_);
        }

        processor_->Step();

        common::Bytes result = processor_->GetAccumulator();

        Reset();

        return result;
    }

    void Exec(const common::OpCode opcode, const common::TypeModifier type_modifier, const common::ArgModifier arg_modifier) const {
        if (arg_modifier == common::ArgModifier::REF) {
            constexpr common::DoubleByte address = 256;
            memory_->WriteInstruction(common::InstructionByte(opcode, type_modifier, arg_modifier), common::Bytes(address));
            memory_->WriteArgument(argument_, address);
        } else if (arg_modifier == common::ArgModifier::REF_REF) {
            constexpr common::DoubleByte address_pointer = 9000;
            constexpr common::DoubleByte address_value = 10000;

            memory_->WriteInstruction(common::InstructionByte(opcode, type_modifier, arg_modifier), common::Bytes(address_pointer));
            memory_->WriteArgument(argument_, address_value);
            memory_->WriteArgument(common::Bytes(address_value), address_pointer);
        } else {
            memory_->WriteInstruction(common::InstructionByte(opcode, type_modifier, arg_modifier), argument_);
        }

        processor_->Step();
    }

    [[nodiscard]] common::Bytes ReadMemory(const common::DoubleByte address) const {
        return memory_->ReadArgument(address);
    }

    // ReSharper disable once CppInconsistentNaming
    [[nodiscard]] common::DoubleByte GetIP() const {
        return processor_->GetInstructionPointer();
    }

    void Reset() const {
        memory_->Reset();
        processor_->Reset();
    }

    void SetUp() override {
        memory_ = std::make_unique<MemoryManager>();
        processor_ = std::make_unique<Processor>(*memory_);
    }
    void TearDown() override {
        Reset();
    }
private:
    std::unique_ptr<MemoryManager> memory_;
    std::unique_ptr<Processor> processor_;
    common::Bytes argument_ = {};
};

INSTANTIATE_TEST_SUITE_P(
    Processor,
    ProcessorTest,
    ::testing::Values(
        common::ArgModifier::NONE,
        common::ArgModifier::REF,
        common::ArgModifier::REF_REF
    ),
    [](const testing::TestParamInfo<common::ArgModifier>& info) {
        switch (info.param) {
            case common::ArgModifier::NONE: return "VALUE";
            case common::ArgModifier::REF: return "REF";
            case common::ArgModifier::REF_REF: return "REF_REF";
            default: return "Unknown";
        }
    }
);

TEST_P(ProcessorTest, Add) {
    constexpr auto opcode = common::OpCode::ADD;
    const auto arg_modifier = GetParam();
    common::Bytes result{};

    SetA(common::Bytes(128));
    SetB(common::Bytes(127));
    result = Calc(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<common::Byte>(result), 255);

    SetA(common::Bytes(1000000000));
    SetB(common::Bytes(1034567892));
    result = Calc(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<common::Word>(result), 2034567892);

    SetA(common::Bytes(std::numeric_limits<common::SignedWord>::min()));
    SetB(common::Bytes(std::numeric_limits<common::SignedWord>::max()));
    result = Calc(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(static_cast<common::SignedWord>(result), -1);

    SetA(common::Bytes(static_cast<common::Real>(1 / 3.0)));
    SetB(common::Bytes(static_cast<common::Real>(2 / 3.0)));
    result = Calc(opcode, common::TypeModifier::R, arg_modifier);
    EXPECT_EQ(static_cast<common::Real>(result), 1);
}

TEST_P(ProcessorTest, Sub) {
    constexpr auto opcode = common::OpCode::SUB;
    const auto arg_modifier = GetParam();
    common::Bytes result{};

    SetA(common::Bytes(128));
    SetB(common::Bytes(127));
    result = Calc(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<common::Byte>(result), 1);

    SetA(common::Bytes(1034567892));
    SetB(common::Bytes(1000000000));
    result = Calc(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<common::Word>(result), 34567892);

    SetA(common::Bytes(std::numeric_limits<common::SignedWord>::min()));
    SetB(common::Bytes(-std::numeric_limits<common::SignedWord>::max()));
    result = Calc(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(static_cast<common::SignedWord>(result), -1);

    SetA(common::Bytes(static_cast<common::Real>(2 / 3.0)));
    SetB(common::Bytes(static_cast<common::Real>(1 / 3.0)));
    result = Calc(opcode, common::TypeModifier::R, arg_modifier);
    EXPECT_EQ(static_cast<common::Real>(result), static_cast<common::Real>(2 / 3.0) - static_cast<common::Real>(1 / 3.0));
}

TEST_P(ProcessorTest, Mul) {
    constexpr auto opcode = common::OpCode::MUL;
    const auto arg_modifier = GetParam();
    common::Bytes result{};

    SetA(common::Bytes(5));
    SetB(common::Bytes(6));
    result = Calc(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<common::Byte>(result), static_cast<common::Byte>(5) * static_cast<common::Byte>(6));

    SetA(common::Bytes(123456));
    SetB(common::Bytes(789012));
    result = Calc(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<common::Word>(result), static_cast<common::Word>(123456) * static_cast<common::Word>(789012));

    SetA(common::Bytes(-12345));
    SetB(common::Bytes(78901));
    result = Calc(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(static_cast<common::SignedWord>(result), -12345 * 78901);

    SetA(common::Bytes(static_cast<common::Real>(2 / 3.0)));
    SetB(common::Bytes(static_cast<common::Real>(1 / 3.0)));
    result = Calc(opcode, common::TypeModifier::R, arg_modifier);
    EXPECT_EQ(static_cast<common::Real>(result), static_cast<common::Real>(2 / 3.0) * static_cast<common::Real>(1 / 3.0));
}

TEST_P(ProcessorTest, Div) {
    constexpr auto opcode = common::OpCode::DIV;
    const auto arg_modifier = GetParam();
    common::Bytes result{};

    SetA(common::Bytes(6));
    SetB(common::Bytes(5));
    result = Calc(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<common::Byte>(result), static_cast<common::Byte>(6) / static_cast<common::Byte>(5));

    SetA(common::Bytes(789012));
    SetB(common::Bytes(123456));
    result = Calc(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<common::Word>(result), static_cast<common::Word>(789012) / static_cast<common::Word>(123456));

    SetA(common::Bytes(78901));
    SetB(common::Bytes(-12345));
    result = Calc(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(static_cast<common::SignedWord>(result), static_cast<common::SignedWord>(78901) / static_cast<common::SignedWord>(-12345));

    SetA(common::Bytes(static_cast<common::Real>(1)));
    SetB(common::Bytes(static_cast<common::Real>(3)));
    result = Calc(opcode, common::TypeModifier::R, arg_modifier);
    EXPECT_EQ(static_cast<common::Real>(result), static_cast<common::Real>(1) / static_cast<common::Real>(3));

    SetA(common::Bytes(1));
    SetB(common::Bytes(0));
    // ReSharper disable once CppNoDiscardExpression
    EXPECT_THROW(Calc(opcode, common::TypeModifier::W, arg_modifier), std::runtime_error);
}

TEST_P(ProcessorTest, Mod) {
    constexpr auto opcode = common::OpCode::MOD;
    const auto arg_modifier = GetParam();
    common::Bytes result{};

    SetA(common::Bytes(6));
    SetB(common::Bytes(5));
    result = Calc(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<common::Byte>(result), static_cast<common::Byte>(6) % static_cast<common::Byte>(5));

    SetA(common::Bytes(789012));
    SetB(common::Bytes(123456));
    result = Calc(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<common::Word>(result), static_cast<common::Word>(789012) % static_cast<common::Word>(123456));

    SetA(common::Bytes(78901));
    SetB(common::Bytes(-12345));
    result = Calc(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(static_cast<common::SignedWord>(result), static_cast<common::SignedWord>(78901) % static_cast<common::SignedWord>(-12345));

    SetA(common::Bytes(static_cast<common::Real>(1)));
    SetB(common::Bytes(static_cast<common::Real>(3)));
    // ReSharper disable once CppNoDiscardExpression
    EXPECT_THROW(Calc(opcode, common::TypeModifier::R, arg_modifier), std::runtime_error);

    SetA(common::Bytes(1));
    SetB(common::Bytes(0));
    // ReSharper disable once CppNoDiscardExpression
    EXPECT_THROW(Calc(opcode, common::TypeModifier::W, arg_modifier), std::runtime_error);
}

TEST_P(ProcessorTest, Load) {
    constexpr auto opcode = common::OpCode::LOAD;
    const auto arg_modifier = GetParam();
    common::Bytes result{};

    SetB(common::Bytes(255));
    result = Calc(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(static_cast<common::Byte>(result), 255);

    SetB(common::Bytes(std::numeric_limits<common::Word>::max()));
    result = Calc(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(static_cast<common::Word>(result), std::numeric_limits<common::Word>::max());

    SetB(common::Bytes(std::numeric_limits<common::SignedWord>::min()));
    result = Calc(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(static_cast<common::SignedWord>(result), std::numeric_limits<common::SignedWord>::min());

    SetB(common::Bytes(std::numeric_limits<common::Real>::max()));
    result = Calc(opcode, common::TypeModifier::R, arg_modifier);
    EXPECT_EQ(static_cast<common::Real>(result), std::numeric_limits<common::Real>::max());
}

TEST_P(ProcessorTest, Store) {
    constexpr auto opcode = common::OpCode::STORE;
    const auto arg_modifier = GetParam();

    if (arg_modifier == common::ArgModifier::REF_REF) {
        GTEST_SKIP() << "Skipping REF_REF for this case";
    }

    common::Bytes result{};
    constexpr common::DoubleByte address = 1234;

    SetA(common::Bytes(std::numeric_limits<common::Real>::max()));
    SetB(common::Bytes(address));
    Exec(opcode, common::TypeModifier::W, arg_modifier);
    result = ReadMemory(address);
    EXPECT_EQ(static_cast<common::Real>(result), std::numeric_limits<common::Real>::max());
}

TEST_P(ProcessorTest, Jump) {
    constexpr auto opcode = common::OpCode::JUMP;
    const auto arg_modifier = GetParam();
    constexpr common::DoubleByte address = 1234;

    SetB(common::Bytes(address));
    Exec(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), address);
}

TEST_P(ProcessorTest, SkipLo_True) {
    constexpr auto opcode = common::OpCode::SKIP_LOWER;
    const auto arg_modifier = GetParam();
    constexpr common::DoubleByte target_ip_value = 2;

    SetA(common::Bytes(1));
    SetB(common::Bytes(5));
    Exec(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(std::numeric_limits<common::Word>::max() - 1));
    SetB(common::Bytes(std::numeric_limits<common::Word>::max()));
    Exec(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(std::numeric_limits<common::SignedWord>::min()));
    SetB(common::Bytes(std::numeric_limits<common::SignedWord>::min() + 1));
    Exec(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(static_cast<common::Real>(3.12)));
    SetB(common::Bytes(static_cast<common::Real>(3.14)));
    Exec(opcode, common::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, SkipLo_False) {
    constexpr auto opcode = common::OpCode::SKIP_LOWER;
    const auto arg_modifier = GetParam();
    constexpr common::DoubleByte target_ip_value = 1;

    SetA(common::Bytes(5));
    SetB(common::Bytes(1));
    Exec(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(std::numeric_limits<common::Word>::max()));
    SetB(common::Bytes(std::numeric_limits<common::Word>::max() - 1));
    Exec(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(std::numeric_limits<common::SignedWord>::min() + 1));
    SetB(common::Bytes(std::numeric_limits<common::SignedWord>::min()));
    Exec(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(static_cast<common::Real>(3.14)));
    SetB(common::Bytes(static_cast<common::Real>(3.12)));
    Exec(opcode, common::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, SkipEq_True) {
    constexpr auto opcode = common::OpCode::SKIP_EQUAL;
    const auto arg_modifier = GetParam();
    constexpr common::DoubleByte target_ip_value = 2;

    SetA(common::Bytes(5));
    SetB(common::Bytes(5));
    Exec(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(std::numeric_limits<common::Word>::max()));
    SetB(common::Bytes(std::numeric_limits<common::Word>::max()));
    Exec(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(std::numeric_limits<common::SignedWord>::min()));
    SetB(common::Bytes(std::numeric_limits<common::SignedWord>::min()));
    Exec(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(static_cast<common::Real>(3.14)));
    SetB(common::Bytes(static_cast<common::Real>(3.14)));
    Exec(opcode, common::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, SkipEq_False) {
    constexpr auto opcode = common::OpCode::SKIP_EQUAL;
    const auto arg_modifier = GetParam();
    constexpr common::DoubleByte target_ip_value = 1;

    SetA(common::Bytes(10));
    SetB(common::Bytes(5));
    Exec(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(123));
    SetB(common::Bytes(std::numeric_limits<common::Word>::max()));
    Exec(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(789));
    SetB(common::Bytes(std::numeric_limits<common::SignedWord>::min()));
    Exec(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(static_cast<common::Real>(3.15)));
    SetB(common::Bytes(static_cast<common::Real>(3.14)));
    Exec(opcode, common::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, SkipGt_True) {
    constexpr auto opcode = common::OpCode::SKIP_GREATER;
    const auto arg_modifier = GetParam();
    constexpr common::DoubleByte target_ip_value = 2;

    SetA(common::Bytes(10));
    SetB(common::Bytes(5));
    Exec(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(std::numeric_limits<common::Word>::max()));
    SetB(common::Bytes(123));
    Exec(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(0));
    SetB(common::Bytes(std::numeric_limits<common::SignedWord>::min()));
    Exec(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(static_cast<common::Real>(3.15)));
    SetB(common::Bytes(static_cast<common::Real>(3.14)));
    Exec(opcode, common::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, SkipGt_False) {
    constexpr auto opcode = common::OpCode::SKIP_GREATER;
    const auto arg_modifier = GetParam();
    constexpr common::DoubleByte target_ip_value = 1;

    SetA(common::Bytes(5));
    SetB(common::Bytes(10));
    Exec(opcode, common::TypeModifier::C, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(123));
    SetB(common::Bytes(std::numeric_limits<common::Word>::max()));
    Exec(opcode, common::TypeModifier::W, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(std::numeric_limits<common::SignedWord>::min()));
    SetB(common::Bytes(789));
    Exec(opcode, common::TypeModifier::SW, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();

    SetA(common::Bytes(static_cast<common::Real>(3.14)));
    SetB(common::Bytes(static_cast<common::Real>(3.15)));
    Exec(opcode, common::TypeModifier::R, arg_modifier);
    EXPECT_EQ(GetIP(), target_ip_value);
    Reset();
}

TEST_P(ProcessorTest, JumpNStore) {
    constexpr auto opcode = common::OpCode::JUMPNSTORE;
    const auto arg_modifier = GetParam();
    constexpr common::DoubleByte target_address = 5;

    SetB(common::Bytes(target_address));
    Exec(opcode, common::TypeModifier::W, arg_modifier);
    // Проверка, что произошел прыжок
    EXPECT_EQ(GetIP(), target_address + 1);
    // Проверка, что был сохранен возвратный адрес
    EXPECT_EQ(static_cast<common::DoubleByte>(ReadMemory(target_address)), 1);
}