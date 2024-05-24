#include <iostream>
#include <string>
#include "prong.h"
#include "prong_integrations.h"

using namespace Prong;
using namespace std;

int main() {
    OpenAIChatModel chat(OpenAIChatModel::ModelType::GPT_3_5_TURBO);
    ChatStream chatStream(chat, cout);

    cout << "You are talking to GPT-3.5 Turbo via a custom stream. Type 'exit' to quit." << endl;

    string userInput;
    while (true) {
        cout << "User: ";
        getline(cin, userInput);
        if (userInput == "exit") {
            break;
        }

        chatStream << userInput << endl; 
    }

    return 0;
}

