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

  template <StringLiteral... keys>
  Message operator()(const Substitution<keys>&... substitutions) const {
    Prompt<template_> p;
    return Message(std::string{messageType}, p(substitutions...));
  }

  template <StringLiteral... keys>
  std::vector<Message> operator()(
      const std::vector<std::tuple<Substitution<keys>...>>& inputs) const {
    std::vector<std::future<Message>> futures;
    for (const std::tuple<Substitution<keys>...>& input : inputs) {
      futures.push_back(std::async(std::launch::async, [this, &input]() {
        Prompt<template_s> p;
        return Message(std::string{messageType},
                       std::apply(
                           [p](Substitution<keys>... substitutions) {
                             return p(substitutions...);
                           },
                           input));
      }));
    }

    std::vector<Message> results;
    for (auto& future : futures) {
      results.push_back(future.get());
    }
    return results;
  }

  template <StringLiteral... keys>
  Message operator()(std::ostream& os,
                     const Substitution<keys>&... substitutions) const {
    Prompt<template_> p;
    Message message(std::string{messageType}, p(substitutions...));
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
