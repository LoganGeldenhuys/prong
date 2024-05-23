#ifndef PRONG_MESSAGE_PROMPT_H
#define PRONG_MESSAGE_PROMPT_H

#include "message.h"
#include "prompt.h"
#include "string_literal.h"

namespace Prong {

// A special type of prompt that returns a Message when passed a substitution
// (not a std::string like Prong::Prompt)
template <StringLiteral messageRole, StringLiteral template_>
struct MessagePrompt {
  static constexpr StringLiteral template_s = template_;
  static constexpr StringLiteral messageType = messageRole;

  template <class... Args>
  Message operator()(Args... args) const {
    Prompt<template_> p;
    return Message(std::string{messageType}, p(std::forward<Args>(args)...));
  }

  template <class... Args>
  std::vector<Message> operator()(
      const std::vector<std::tuple<Args...>>& inputs) const {
    std::vector<std::future<Message>> futures;
    for (const std::tuple<Args...>& input : inputs) {
      futures.push_back(std::async(std::launch::async, [this, &input]() {
        Prompt<template_s> p;
        return Message(std::string{messageType}, p(input));
      }));
    }

    std::vector<Message> results;
    for (auto& future : futures) {
      results.push_back(future.get());
    }
    return results;
  }

  template <class... Args>
  Message operator()(std::ostream& os, Args... args) const {
    Prompt<template_> p;
    Message message(std::string{messageType}, p(std::forward<Args>(args)...));
    os << message;
    return message;
  }

  template <class NewOutput>
  PromptRunnable<MessagePrompt<messageRole, template_>, Message, NewOutput>
  operator|(Runnable<Message, NewOutput>& newRight) {
    return PromptRunnable<Prompt<template_s>, Message, NewOutput>(*this,
                                                                  newRight);
  }
};
}  // namespace Prong

#endif
