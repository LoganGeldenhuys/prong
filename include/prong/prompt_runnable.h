#ifndef PRONG_PROMPT_RUNNABLE_H
#define PRONG_PROMPT_RUNNABLE_H

#include "runnable.h"
#include "string_literal.h"
#include "substitution.h"

namespace Prong {

// A prompt type that is created by combining another prompt type with a
// Runnable. Returns whatever the output of its Runnable is when passed
// Substitutions.
template <class PromptType, class Intermediate, class Output>
class PromptRunnable {
  PromptType left_;
  Runnable<Intermediate, Output>* right_;

 public:
  PromptRunnable(PromptType prompt, Runnable<Intermediate, Output>& right)
      : left_(prompt), right_(&right) {}

  template <StringLiteral... keys>
  Output operator()(const Substitution<keys>&... substitutions) {
    return (*right_)(left_(substitutions...));
  }

  template <StringLiteral... keys>
  std::vector<Output> operator()(
      const std::vector<std::tuple<Substitution<keys>...>>& inputs) {
    std::vector<std::future<Output>> futures;
    for (const std::tuple<Substitution<keys>...> input : inputs) {
      futures.push_back(std::async(std::launch::async, [this, input]() {
        return std::apply(
            [this](const Substitution<keys>&... substitutions) {
              return (*(this->right_))(
                  this->left_(substitutions...));
            },
            input);
      }));
    }
    std::vector<Output> results;
    for (std::future<Output>& future : futures) {
      results.push_back(future.get());
    }
    return results;
  }

  template <StringLiteral... keys>
  Output operator()(std::ostream& os, const Substitution<keys>&... substitutions) {
    return (*right_)(os, left_(substitutions...));
  };

  template <class NewOutput>
  PromptRunnable<PromptType, Output, NewOutput> operator|(
      Runnable<Output, NewOutput>& runnable) {
    return PromptRunnable<PromptType, Output, NewOutput>(*this, runnable);
  }
};

}  // namespace Prong

#endif  // PRONG_PROMPT_RUNNABLE_H
