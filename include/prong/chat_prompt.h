#ifndef PRONG_CHAT_PROMPT_H
#define PRONG_CHAT_PROMPT_H

#include "template_string.h"
#include "string_literal.h"
#include "message.h"
#include "prompt.h"
#include "message_prompt.h"


namespace Prong{
	
	// A collection of MessagePrompts that outputs a std::vector of Messages when passed Substitutions
	template <MessagePrompt... messagePrompts>
	class ChatPrompt {
		public:
		static void print(){
			(printMP<messagePrompts>(),...);
		}

		template <MessagePrompt mp>
		static void printMP(){

		std::cout<<mp.messageType<<":"<<std::string_view{mp.template_s}<<std::endl;
		
		}

		template<class ...Args>
		std::vector<Message> operator()(Args ...args) const {
			std::vector<Message> results;
			results.reserve(sizeof...(messagePrompts));
			([&results, args...](auto& messagePrompt) {
				results.push_back(messagePrompt(args...));
				}(messagePrompts), ...);
			return results;
		}
		
		template<class ...Args>
		std::vector<std::vector<Message>> operator()(const std::vector<std::tuple<Args...>>& inputs) const {
			std::vector<std::vector<Message>> results;
			results.reserve(inputs.size());

			for (const auto& input : inputs) {
				std::vector<Message> batchResults;
				batchResults.reserve(sizeof...(messagePrompts));
				
				([&batchResults, &input](auto& messagePrompt) {
					batchResults.push_back(std::apply(messagePrompt, input));
				}(messagePrompts), ...);

				results.push_back(std::move(batchResults));
			}

			return results;
		}

		template<class ...Args>
		std::vector<Message> operator()(std::ostream& os, Args ...args) const {
			std::vector<Message> results;
			results.reserve(sizeof...(messagePrompts));

			([&results, &os, &args...](auto& messagePrompt) {
				results.push_back(messagePrompt(os, std::forward<Args>(args)...));
			}(messagePrompts), ...);

			return results;
		}

		template<class NewOutput>
		PromptRunnable<ChatPrompt<messagePrompts...>, std::vector<Message>, NewOutput> operator|(Runnable<std::vector<Message>, NewOutput>& newRight) {
			return PromptRunnable<ChatPrompt<messagePrompts...>, std::vector<Message>, NewOutput>(*this, newRight);
		}

	};

} // Prong

#endif //PRONG_CHAT_PROMPT_H
