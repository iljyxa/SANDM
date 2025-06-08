#include <gtest/gtest.h>

#include "core/common_definitions.hpp"

TEST(BytesTest, Byte) {
    constexpr snm::Byte value = std::numeric_limits<snm::Byte>::max();
    const snm::Bytes bytes{value};
    EXPECT_EQ(static_cast<snm::Byte>(bytes), value);
}

TEST(BytesTest, Word) {
    constexpr snm::Word value = std::numeric_limits<snm::Word>::max();
    const snm::Bytes bytes{value};
    EXPECT_EQ(static_cast<snm::Word>(bytes), value);
}

TEST(BytesTest, SignedWord) {
    constexpr snm::SignedWord value = std::numeric_limits<snm::SignedWord>::max();
    const snm::Bytes bytes{value};
    EXPECT_EQ(static_cast<snm::SignedWord>(bytes), value);
}

TEST(BytesTest, Real) {
    constexpr snm::Real value = std::numeric_limits<snm::Real>::max();
    const snm::Bytes bytes{value};
    EXPECT_EQ(static_cast<snm::Real>(bytes), value);
}