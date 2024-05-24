#ifndef PRONG_RUNNABLE_H
#define PRONG_RUNNABLE_H

#include <future>
#include <iostream>
#include <string>
#include <vector>
#include "./message.h"

namespace Prong {

// Core runnable class that handles parallelization of sub-classes such as
// ChatModel. Also has a utility streaming operator() method that can be
// overloaded for different behavior
template <class Input, class Output>
class Runnable {
 private:
  virtual Output run(const Input& input) const = 0;

  virtual Output runAndStream(std::ostream& os, const Input& input) const {
    Output result = (*this)(input);
    os << result;
    return result;
  }

 public:
  // Process a single input using run method
  virtual Output operator()(const Input& input) const final {
    return run(input);
  }

  // Process a list of inputs and return a vector of futures using call method
  virtual std::vector<Output> operator()(
      const std::vector<Input>& inputs) const final {
    std::vector<std::future<Output>> futures;
    for (const auto& input : inputs) {
      futures.push_back(std::async(std::launch::async,
                                   [this, input]() { return (*this)(input); }));
    }

    std::vector<Output> results;
    for (std::future<Output>& f : futures) {
      results.push_back(f.get());
    }
    return results;
  }

  // Process a single input and stream the result to the provided
  // stream using call method
  virtual Output operator()(std::ostream& os, const Input& input) const final {
    return runAndStream(os, input);
  }
};

using LanguageModel = Runnable<std::string, std::string>;
using ChatModel = Runnable<std::vector<Message>, Message>;

}  // namespace Prong

#endif  // PRONG_RUNNABLE_H
