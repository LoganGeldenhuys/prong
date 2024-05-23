#include <gtest/gtest.h>
#include <tuple>
#include "prong.h"  // Assumes that all relevant headers are included here

using namespace Prong;

template <typename T, typename = void>
struct does_compile : std::false_type {};


TEST(PromptTest, OutputCorrectness) {
    Prompt<"Hello {name}, what is up with {topic}"> helloPrompt;
    Substitution<"name"> nameSub("John");
    Substitution<"topic"> topicSub("programming");

    std::string result = helloPrompt(nameSub, topicSub);
    EXPECT_EQ(result, "Hello John, what is up with programming");
}

TEST(PromptTest, BatchInput) {
    Prompt<"Write a {style} limerick about {topic}"> limerickPrompt;
    Substitution<"topic"> topicSub1("Autumn");
    Substitution<"style"> styleSub1("Victorian");

    std::vector<std::tuple<Substitution<"topic">, Substitution<"style">>> inputs = {
        {topicSub1, styleSub1}
    };

    std::vector<std::string> results = limerickPrompt(inputs);
    EXPECT_EQ(results[0], "Write a Victorian limerick about Autumn");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

