#include <gtest/gtest.h>
#include "prong.h"

using namespace Prong;

// Test to ensure that Substitution holds and returns the correct value
TEST(SubstitutionTest, CorrectValue) {
    Substitution<"username"> usernameSub("john_doe");

    EXPECT_EQ(usernameSub.value, "john_doe");
    EXPECT_STREQ(usernameSub.my_key_c, "username");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

