#include <gtest/gtest.h>
#include <sstream>
#include "prong.h" 

using namespace Prong;
using namespace std;

template <StringLiteral... keys>
class MockPrompt {
public:
    template <typename... Substitutions>
    string operator()(const Substitutions&... subs) const {
        ostringstream os;
        (os <<  ... << (subs.value+" "));
        return os.str();
    }
};

// Mock Runnable which transforms a string by appending " processed"
class MockStringProcessRunnable : public Runnable<string, string> {
  virtual string run(const string& input) const override {
    return input + "processed";
  }

  virtual string runAndStream(ostream& os, const string& input) const override {
    string result = run(input);
    os << result;
    return result;
  }
};

TEST(PromptRunnableTest, BasicFunctionality) {
    MockPrompt<"name", "action"> prompt;
    MockStringProcessRunnable processRunnable;

    PromptRunnable<decltype(prompt), string, string> promptRunnable(prompt, processRunnable);
    Substitution<"name"> name("John");
    Substitution<"action"> action("jump");

    auto result = promptRunnable(name, action);
    EXPECT_EQ(result, "John jump processed");
}

TEST(PromptRunnableTest, BatchProcessing) {
    MockPrompt<"item"> prompt;
    MockStringProcessRunnable processRunnable;

    PromptRunnable<decltype(prompt), string, string> promptRunnable(prompt, processRunnable);
    vector<tuple<Substitution<"item">>> inputs = {
        make_tuple(Substitution<"item">("apple")),
        make_tuple(Substitution<"item">("banana"))
    };

    auto results = promptRunnable(inputs);
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(results[0], "apple processed");
    EXPECT_EQ(results[1], "banana processed");
}

TEST(PromptRunnableTest, StreamingOutput) {
    MockPrompt<"task"> prompt;
    MockStringProcessRunnable processRunnable;

    PromptRunnable<decltype(prompt), string, string> promptRunnable(prompt, processRunnable);
    Substitution<"task"> task("write tests");

    stringstream ss;
    auto result = promptRunnable(ss, task);
    EXPECT_EQ(result, "write tests processed");
    EXPECT_EQ(ss.str(), "write tests processed");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

