#include <gtest/gtest.h>
#include <sstream>
#include "prong.h"  // Includes all necessary headers from the Prong namespace

using namespace Prong;
using namespace std;

// Mock implementation of Runnable for converting string to int (simulated)
class MockStringToIntRunnable : public Runnable<string, int> {
  virtual int run(const string& input) const override {
    return input.length();  // Returns the length of the string as an "int processing"
  }

  virtual int runAndStream(ostream& os, const string& input) const override {
    int result = run(input);
    os << "Length: " << result;
    return result;
  }
};

// Mock implementation of Runnable for converting int to string
class MockIntToStringRunnable : public Runnable<int, string> {
  virtual string run(const int& input) const override {
    return "Processed to string: " + to_string(input);
  }

};

TEST(CombinedRunnableTest, BasicFunctionality) {
    MockStringToIntRunnable stringToInt;
    MockIntToStringRunnable intToString;

    CombinedRunnable<string, int, string> combined(stringToInt, intToString);
    string input = "test";
    auto result = combined(input);
    EXPECT_EQ(result, "Processed to string: 4");
}

TEST(CombinedRunnableTest, BatchProcessing) {
    MockStringToIntRunnable stringToInt;
    MockIntToStringRunnable intToString;

    CombinedRunnable<string, int, string> combined(stringToInt, intToString);
    vector<string> inputs = {"hello", "world", "test"};
    auto results = combined(inputs);

    ASSERT_EQ(results.size(), 3);
    EXPECT_EQ(results[0], "Processed to string: 5");
    EXPECT_EQ(results[1], "Processed to string: 5");
    EXPECT_EQ(results[2], "Processed to string: 4");
}

TEST(CombinedRunnableTest, StreamingOutput) {
    MockStringToIntRunnable stringToInt;
    MockIntToStringRunnable intToString;

    CombinedRunnable<string, int, string> combined(stringToInt, intToString);
    string input = "stream";
    stringstream ss;
    auto result = combined(ss, input);

    EXPECT_EQ(result, "Processed to string: 6");
    EXPECT_EQ(ss.str(), "Processed to string: 6");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

