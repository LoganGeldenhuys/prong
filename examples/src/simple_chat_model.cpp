#include <iostream>
#include <vector>
#include "prong.h"

// This example shows how to create a custom ChatModel and use it

using namespace Prong;
using namespace std;

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
  vector<Message> conversationHistory;
  SimpleChatModel chat{};

  std::cout << "You are talking to a simple chat model, type exit to quit"
            << std::endl;

  while (true) {
    string userMessage;
    cout << "user: ";
    getline(cin, userMessage);
    if (userMessage == "exit") {
      break;
    }
    conversationHistory.push_back(Message("user", userMessage));
    Message response = chat(conversationHistory);
    cout << endl << response << endl;
    conversationHistory.push_back(response);
  }

  return 0;
}
