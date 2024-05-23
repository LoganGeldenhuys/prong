#ifndef PRONG_CHAT_PROMPT_H
#define PRONG_CHAT_PROMPT_H

#include "message.h"
#include "message_prompt.h"

namespace Prong {

// A collection of MessagePrompts that outputs a std::vector of Messages when
// passed Substitutions
template <MessagePrompt... messagePrompts>
class ChatPrompt {
 public:
  static void print() { (printMP<messagePrompts>(), ...); }

  template <MessagePrompt mp>
  static void printMP() {
    std::cout << mp.messageType << ":" << std::string_view{mp.template_s}
              << std::endl;
  }

  template <StringLiteral... keys>
  std::vector<Message> operator()(const Substitution<keys>&... substitutions) const {
    std::vector<Message> results;
    results.reserve(sizeof...(messagePrompts));
    (
        [&results, substitutions...](auto& messagePrompt) {
          results.push_back(messagePrompt(substitutions...));
        }(messagePrompts),
        ...);
    return results;
  }

  template <StringLiteral... keys>
  std::vector<std::vector<Message>> operator()(
      const std::vector<std::tuple<Substitution<keys>...>>& inputs) const {
    std::vector<std::vector<Message>> results;
    results.reserve(inputs.size());

    for (const auto& input : inputs) {
      std::vector<Message> batchResults;
      batchResults.reserve(sizeof...(messagePrompts));

      (
          [&batchResults, &input](auto& messagePrompt) {
            batchResults.push_back(std::apply(messagePrompt, input));
          }(messagePrompts),
          ...);

      results.push_back(std::move(batchResults));
    }

    return results;
  }

  template <StringLiteral... keys>
  std::vector<Message> operator()(std::ostream& os, const Substitution<keys>&... substitutions) const {
    std::vector<Message> results;
    results.reserve(sizeof...(messagePrompts));

    (
        [&results, &os, &substitutions...](auto& messagePrompt) {
          results.push_back(messagePrompt(os, substitutions...));
        }(messagePrompts),
        ...);

    return results;
  }

  template <class NewOutput>
  PromptRunnable<ChatPrompt<messagePrompts...>, std::vector<Message>, NewOutput>
  operator|(Runnable<std::vector<Message>, NewOutput>& newRight) {
    return PromptRunnable<ChatPrompt<messagePrompts...>, std::vector<Message>,
                          NewOutput>(*this, newRight);
  }
};

}  // namespace Prong

#endif  // PRONG_CHAT_PROMPT_H
