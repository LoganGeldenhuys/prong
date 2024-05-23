#include <gtest/gtest.h>
#include <sstream>
#include "prong.h"

using namespace Prong;
using namespace std;

MessagePrompt<
    "system",
    "You are {character}. Reply only as {character} talking about {question}.">
    systemMessage;
MessagePrompt<"user", "Hey {character}, what do you think about {question}?">
    questionMessage;
ChatPrompt<systemMessage, questionMessage> characterAnalysisPrompt;

TEST(ChatPromptTest, SingleInput) {
  Substitution<"character"> character("Darth Vader");
  Substitution<"question"> question("the force");

  auto messages = characterAnalysisPrompt(character, question);
  EXPECT_EQ(messages.size(), 2);
  EXPECT_EQ(messages[0].content,
            "You are Darth Vader. Reply only as Darth Vader talking about the "
            "force.");
  EXPECT_EQ(messages[1].content,
            "Hey Darth Vader, what do you think about the force?");
}

// tests with normal vector
TEST(ChatPromptTest, BatchInput) {
  vector<tuple<Substitution<"character">, Substitution<"question">>> inputs = {
      {Substitution<"character">("Luke"),
       Substitution<"question">("your father")},
      {Substitution<"character">("Leia"),
       Substitution<"question">("the rebellion")}};

  auto batchMessages = characterAnalysisPrompt(inputs);
  EXPECT_EQ(batchMessages.size(), 2);
  EXPECT_EQ(batchMessages[0][0].content,
            "You are Luke. Reply only as Luke talking about your father.");
  EXPECT_EQ(batchMessages[0][1].content,
            "Hey Luke, what do you think about your father?");
  EXPECT_EQ(batchMessages[1][0].content,
            "You are Leia. Reply only as Leia talking about the rebellion.");
  EXPECT_EQ(batchMessages[1][1].content,
            "Hey Leia, what do you think about the rebellion?");
}

// tests with SubstitutionVector
TEST(ChatPromptTest, BatchInputSubstitutionVector) {
  SubstitutionVector<"character", "question"> inputs = {
      {Substitution<"character">("Luke"),
       Substitution<"question">("your father")},
      {Substitution<"character">("Leia"),
       Substitution<"question">("the rebellion")}};

  auto batchMessages = characterAnalysisPrompt(inputs);
  EXPECT_EQ(batchMessages.size(), 2);
  EXPECT_EQ(batchMessages[0][0].content,
            "You are Luke. Reply only as Luke talking about your father.");
  EXPECT_EQ(batchMessages[0][1].content,
            "Hey Luke, what do you think about your father?");
  EXPECT_EQ(batchMessages[1][0].content,
            "You are Leia. Reply only as Leia talking about the rebellion.");
  EXPECT_EQ(batchMessages[1][1].content,
            "Hey Leia, what do you think about the rebellion?");
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
