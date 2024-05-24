#include <gtest/gtest.h>
#include "prong.h"  

using namespace Prong;

// Test to ensure that StringLiteral holds and compares strings correctly
TEST(StringLiteralTest, CorrectInstantiationAndComparison) {
    constexpr StringLiteral<6> greeting("hello");
    constexpr StringLiteral<6> sameGreeting("hello");
    constexpr StringLiteral<6> differentGreeting("world");

    // Test equality
    EXPECT_EQ(greeting, sameGreeting);
    EXPECT_NE(greeting, differentGreeting);

    // Test compile-time properties
    static_assert(greeting.size == 6, "Size should be correct at compile time");
    static_assert(greeting.value[0] == 'h', "Value should be initialized correctly");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

