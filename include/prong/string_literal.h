#ifndef PRONG_TEMPLATE_STRING_H
#define PRONG_TEMPLATE_STRING_H

#include <algorithm>
#include <array>
#include <compare>
#include <cstddef>
#include <string>

namespace Prong {

// A custom data type that allows us to instantiate prompts from TemplateStrings
// Will add some more static type safety features unique to TemplateStrings down
// the line but is currently basically the same as StringLiteral
template <std::size_t N>
struct StringLiteral {
  size_t size = N;
  constexpr StringLiteral(const char (&str)[N]) {
    std::copy_n(str, N, value.begin());
  }
  std::array<char, N> value;

  constexpr operator char const*() const { return value.data(); }

  constexpr auto operator<=>(const StringLiteral& other) const {
    for (size_t i = 0; i != N; ++i) {
      if (value[i] != other.buf[i]) {
        return value[i] <=> other.buf[i];
      }
    }
    return std::strong_ordering::equal;
  }
};

template <size_t N>
StringLiteral(char const (&)[N]) -> StringLiteral<N>;

}  // namespace Prong

#endif
