#include <gtest/gtest.h>
#include <sstream>
#include "prong.h" 

using namespace Prong;
using namespace std;

// Mock implementation of Runnable for strings
class MockRunnable : public Runnable<string, string> {
  // Asynchronously process a single input using call method
  virtual string run(const string& input) const override {
    return input + " processed";
  }
};

TEST(RunnableTest, SingleInput) {
    MockRunnable runnable;
    string input = "test";
    auto result = runnable(input);
    EXPECT_EQ(result, "test processed");
}

TEST(RunnableTest, MultipleInputAsync) {
    MockRunnable runnable;
    vector<string> inputs = {"test1", "test2", "test3"};
    auto results = runnable(inputs);
    ASSERT_EQ(results.size(), 3);
    EXPECT_EQ(results[0], "test1 processed");
    EXPECT_EQ(results[1], "test2 processed");
    EXPECT_EQ(results[2], "test3 processed");
}

TEST(RunnableTest, SingleInputWithStream) {
    MockRunnable runnable;
    string input = "stream test";
    stringstream ss;
    auto result = runnable(ss, input);
    EXPECT_EQ(result, "stream test processed");
    EXPECT_EQ(ss.str(), "stream test processed");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

