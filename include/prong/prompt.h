#ifndef PRONG_PROMPT_H
#define PRONG_PROMPT_H

#include <string>
#include <unordered_map>
#include <future>
#include <vector>
#include <variant>
#include <iostream>
#include <regex>
#include <array>
#include <string_view>
#include <type_traits>
#include <utility>
#include <any>
#include <utility>
#include "template_string.h"
#include "string_literal.h"
#include "substitution.h"
#include "apply_to_tuple.h"
#include "runnable.h"
#include "prompt_runnable.h"


namespace Prong {

// The base prompt type. Handles placing Substitutions into template strings. Also checks whether Substitutions are valid. Returns a simple std::string when passed Substitutions.
template<TemplateString template_s> 
class Prompt {

	private:

		static constexpr std::string_view template_{template_s.value.data(), template_s.value.size()};

		// Counts placeholders in template string 
		static constexpr size_t countPlaceholders() {
		   size_t count = 0;
		   for (size_t i = 0; i < template_.size(); ++i) {
			   if (template_[i] == '{') ++count; 
		   }
		   return count;
		}

		static constexpr size_t numPlaceholders = countPlaceholders();

		// Extracts placeholder from string_view
		static constexpr std::string_view extractPlaceholder(std::string_view str, size_t& start) {
			size_t end = start;
			while (end < str.size() && str[end] != '}') ++end;
			auto placeholder = std::string_view{str.data() + start, end - start}; 
			start = end + 1; 
			return placeholder;
		}

		// Uses extractPlaceholder to extract all placeholders from TemplateString
		template<size_t... I>
		static constexpr auto extractPlaceholders(std::index_sequence<I...>) {
			size_t start = 0;
			std::array<std::string_view, sizeof...(I)> placeholders{};
			((start = template_.find('{', start), placeholders[I] = extractPlaceholder(template_, ++start)), ...);
			return placeholders;
		}
		
		// Creates index sequence and calls extractPlaceholders
		static constexpr auto extractPlaceholders() {
		   constexpr size_t count = countPlaceholders();
		   return extractPlaceholders(std::make_index_sequence<count>{});
		}

	public:
		static constexpr std::array<std::string_view, numPlaceholders> placeholders = extractPlaceholders();
	private:

		// Recursive template to check if T matches any string_view in placeholders.
		template <size_t I, size_t N, StringLiteral<N> F>
		static constexpr bool containedInPlaceholders() {
			if constexpr (I < numPlaceholders) {
				if (std::string_view(F) == placeholders[I]) {
					return true;
				} else {
					return containedInPlaceholders<I + 1, N, F>();
				}
			}
			return false;
		}

		// Format that swaps out placeholder with string value
		template<size_t N, StringLiteral<N> S>
		void format(const std::string value, std::string& targetString) const {
			static_assert(containedInPlaceholders<0, N, S>(), "PromptInput key does not match any placeholder in TemplateString");
			std::regex varRegex(R"(\{\s*)" + std::string(S) + R"(\s*\})"); 
			targetString = std::regex_replace(targetString, varRegex, value); 
		}
		 
	public:
		explicit Prompt(){}

		template<class ...Args>
		std::string operator()(Args ...args) const{
			std::string output{template_};
			(format<args.template my_key.size, args.template my_key>(args.value, output), ...);
			return output;
		}

		template<class ...Args>
		std::vector<std::string> operator()(const std::vector<std::tuple<Args...>>& inputs) const {
			std::vector<std::future<std::string>> futures;
			for (const std::tuple<Args...>& input : inputs) {
				futures.push_back(std::async(std::launch::async, [this, &input]() {
				return applyToTuple([this](auto&&... args) {
					return this->operator()(std::forward<decltype(args)>(args)...);
				}, input);
				}));
			}

			std::vector<std::string> results;
			for (auto& future : futures) {
				results.push_back(future.get()); 
			}
			return results;
		}

		template<class ...Args>
		std::string operator()(std::ostream& os, Args ...args) const{
			std::string output{template_};
			(format<args.template my_key.size, args.template my_key>(args.value, output), ...);
			os << output; 
			return output;
		}

		template<class NewOutput>
		PromptRunnable<Prompt<template_s>, std::string, NewOutput> operator|(Runnable<std::string, NewOutput>& newRight) {
			return PromptRunnable<Prompt<template_s>, std::string, NewOutput>(*this, newRight);
		}

};

} // namespace Prong

#endif // PRONG_PROMPT_H

