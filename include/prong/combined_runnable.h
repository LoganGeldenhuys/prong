#ifndef PRONG_COMBINED_RUNNABLE_H
#define PRONG_COMBINED_RUNNABLE_H

#include <concepts>
#include <future>
#include <iostream>
#include <tuple>
#include <vector>
#include "prompt.h"
#include "runnable.h"

namespace Prong {

// A container to house chained runnables after the use of the | operator
// The difference between CombinedRunnable and PromptRunnable is that
// CombinedRunnable starts with a pure runnable and PromptRunnable with a prompt
// type
template <typename Input, typename Intermediate, typename Output>
class CombinedRunnable : public Runnable<Input, Output> {
  Runnable<Input, Intermediate>* left_;
  Runnable<Intermediate, Output>* right_;

  virtual Output run(const Input& input) const override {
    return (*right_)((*left_)(input));
  }

 public:
  CombinedRunnable(Runnable<Input, Intermediate>& left,
                   Runnable<Intermediate, Output>& right)
      : left_(&left), right_(&right) {}

  template <typename NewOutput>
  CombinedRunnable<Input, Output, NewOutput> operator|(
      Runnable<Output, NewOutput>& newRight) {
    return CombinedRunnable<Input, Output, NewOutput>(*this, newRight);
  }
};

}  // namespace Prong

#endif  // PRONG_COMBINED_RUNNABLE_H
