#include <iostream>
#include <vector>
#include <string>
#include "prong.h"
#include "prong_integrations.h"

//This example shows how to interact with OpenAI's chat models.

using namespace Prong;
using namespace std;

int main() {
    vector<Message> conversationHistory;
    OpenAIChatModel chat(OpenAIChatModel::ModelType::GPT_3_5_TURBO);

    std::cout << "You are talking to GPT3.5 turbo, type exit to quit" << std::endl;

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
