#include <algorithm>
#include "prong.h"
#include "prong_integrations.h"

using namespace Prong;
using namespace std;

vector<string> months = {"January",   "February", "March",    "April",
                         "May",       "June",     "July",     "August",
                         "September", "October",  "November", "December"};

class SimpleChatModel : public ChatModel {
 public:
  virtual Message operator()(const std::vector<Message>& input) const override {
    if (!input.empty()) {
      return Message("assistant", input[input.size() - 1].content);
    }
    return Message("AI", "");
  }
};

int main() {
  // Set up prompts
  MessagePrompt<
      "system",
      "The topic is the month of {topic}.\n The user instruction is: {input}">
      systemMessage;

  ChatPrompt<systemMessage> prompt;

  OpenAIChatModel gpt(OpenAIChatModel::ModelType::GPT_3_5_TURBO);

  // create chain
  auto chain = prompt | gpt;

  // Get user input
  cout << "This will generate an type of output for every month of the year. "
          "Please provide an instruction, e.g. \"Write a poem\" or \"Give me "
          "suggestions for an international trip\" "
       << endl;
  string userInput;
  getline(cin, userInput);

  // Generate substitutions
  SubstitutionVector<"topic", "input"> input;

  for (auto month : months) {
    input.push_back(std::tuple(Substitution<"topic">(month),
                               Substitution<"input">(userInput)));
  }

  // run chain (executes prompts in parallel behind the scenes)
  vector<Message> outputs = chain(input);

  // print outputs
  auto month = months.begin();
  for (Message output : outputs) {
    cout << *month++ << ":\n" << output.content << "\n" << endl;
  }
}
