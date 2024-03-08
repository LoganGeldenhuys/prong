#ifndef PRONG_APPLY_TO_TUPLE
#define PRONG_APPLY_TO_TUPLE

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

// Takes a tuple and applies it to a templated function that takes (Args... args) as arguments
namespace Prong{

	//Credit to PeterSom, Sep 27, 2013 at 21:54, https://stackoverflow.com/a/19060157

	template<typename F, typename Tuple, std::size_t ... I>
	auto applyToTupleImpl(F&& f, Tuple&& t, std::index_sequence<I...>) {
		return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
	}
	
	template<typename F, typename Tuple>
	auto applyToTuple(F&& f, Tuple&& t) {
		using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
		return applyToTupleImpl(std::forward<F>(f), std::forward<Tuple>(t), Indices());
	}

} //Prong

#endif //PRONG_APPLY_TO_TUPLE
