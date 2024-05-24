#include <gtest/gtest.h>
#include <sstream>
#include "prong.h"  // Include all relevant Prong namespace headers

using namespace Prong;
using namespace std;

vector<string> months = {"January",   "February", "March",    "April",
                         "May",       "June",     "July",     "August",
                         "September", "October",  "November", "December"};



// Test for basic functionality of MessagePrompt
TEST(MessagePromptTest, BasicFunctionality) {
    // Setup message prompt with placeholders for role and template
    MessagePrompt<"admin", "Hello, {name}! Your account is {status}."> adminGreeting;

    // Provide substitutions that match the keys in the prompt template
    Substitution<"name"> nameSub("John Doe");
    Substitution<"status"> statusSub("active");

    // Generate the message
    Message message = adminGreeting(nameSub, statusSub);

    // Check the results
    EXPECT_EQ(message.role, "admin");
    EXPECT_EQ(message.content, "Hello, John Doe! Your account is active.");
}

// Test for batch processing of multiple inputs
TEST(MessagePromptTest, BatchProcessing) {
	  MessagePrompt<
      "system",
      "The topic is the month of {topic}.\n The user instruction is: {input}">
      systemMessage;

	    // Generate substitutions
  SubstitutionVector<"topic", "input"> input;

  for (auto month : months) {
    input.push_back(std::tuple(Substitution<"topic">(month),
                               Substitution<"input">("hello")));
  }


   MessagePrompt<"user", "Update {item}: Status changed to {status}."> updatePrompt;

   // Vector of tuple inputs for batch processing
   SubstitutionVector<"item","status"> inputs = {
	   make_tuple(Substitution<"item">("Order"), Substitution<"status">("shipped")),
	   make_tuple(Substitution<"item">("Payment"), Substitution<"status">("completed"))
   };

   // Process the batch of inputs
   auto messages = updatePrompt(inputs);

   // Check the results
   ASSERT_EQ(messages.size(), 2);
   EXPECT_EQ(messages[0].content, "Update Order: Status changed to shipped.");
   EXPECT_EQ(messages[1].content, "Update Payment: Status changed to completed.");
}

// Test for streaming output of a message
TEST(MessagePromptTest, StreamingOutput) {
   MessagePrompt<"support", "Ticket {id}: {issue}"> ticketPrompt;

   Substitution<"id"> ticketId("1234");
   Substitution<"issue"> issueDesc("connection problem");

   stringstream ss;
   Message message = ticketPrompt(ss, ticketId, issueDesc);

   // Check the streamed output
   EXPECT_EQ(message.role, "support");
   EXPECT_EQ(message.content, "Ticket 1234: connection problem");
   EXPECT_EQ(ss.str(), "support: Ticket 1234: connection problem\n");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

