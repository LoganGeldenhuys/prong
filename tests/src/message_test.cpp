#include "prong.h"
#include <gtest/gtest.h>
#include <sstream>

namespace Prong {

TEST(MessageTest, ConstructorTest) {
    Message msg("User", "Hello, world!");
    EXPECT_EQ(msg.role, "User");
    EXPECT_EQ(msg.content, "Hello, world!");
}

TEST(MessageTest, OutputStreamTest) {
    Message msg("Admin", "Test message");
    std::ostringstream os;
    os << msg;
    EXPECT_EQ("Admin: Test message\n", os.str());
}

}  // namespace Prong

