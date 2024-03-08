#ifndef PRONG_PROMPT_RUNNABLE_H
#define PRONG_PROMPT_RUNNABLE_H

#include "runnable.h"
#include "template_string.h"
#include "prompt.h"
#include "apply_to_tuple.h"

namespace Prong{

// A prompt type that is created by combining another prompt type with a Runnable. Returns whatever the output of its Runnable is when passed Substitutions.
template<class PromptType, class Intermediate, class Output>
class PromptRunnable {
	PromptType left_;
	Runnable<Intermediate, Output>* right_;

	public:

	PromptRunnable(PromptType prompt, Runnable<Intermediate, Output>& right) : left_(prompt), right_(&right) {}
		

		template<class ...Args>
		Output operator()(Args ...args) {
			return (*right_)(left_(std::forward<Args>(args)...));
		}

		template<class ...Args>
		std::vector<Output> operator()(const std::vector<std::tuple<Args...>>& inputs) {
			std::vector<std::future<Output>> futures;
			for (const std::tuple<Args...> input : inputs) {
				futures.push_back(std::async(std::launch::async, [this, input]() {
				return applyToTuple([this](auto&&... args) {
					return (*(this->right_))(this->left_(std::forward<decltype(args)>(args)...));
				}, input);
			}));
			}
			std::vector<Output> results;
			for (std::future<Output>& future : futures) {
				results.push_back(future.get());
			}
			return results;
		}

		template<class ...Args>
		Output operator()(std::ostream& os, Args ...args) {
			return (*right_)(os, left_(std::forward<Args>(args)...));
		};
		
		template<class NewOutput>
		PromptRunnable<PromptType, Output, NewOutput> operator|(Runnable<Output, NewOutput>& runnable) {
			return PromptRunnable<PromptType, Output, NewOutput>(*this, runnable);
		}

};

} //Prong
  
#endif //PRONG_PROMPT_RUNNABLE_H
