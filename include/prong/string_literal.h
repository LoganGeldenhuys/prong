#ifndef PRONG_STRING_LITERAL_H
#define PRONG_STRING_LITERAL_H

#include <compare>
#include <cstddef>

namespace Prong {

// A custom data type that allows us to use string literals as template
// parameters credit to
// https://www.reddit.com/r/cpp/comments/bhxx49/comment/elwmdjp/
template <size_t n>
struct StringLiteral {
  char buf[n + 1]{};
  size_t size = n;
  constexpr StringLiteral(char const* s) {
    for (size_t i = 0; i != n; ++i)
      buf[i] = s[i];
    buf[n] = '\0';
  }
  constexpr operator char const*() const { return buf; }

  constexpr auto operator<=>(const StringLiteral& other) const {
    for (size_t i = 0; i != n; ++i) {
      if (buf[i] != other.buf[i]) {
        return buf[i] <=> other.buf[i];
      }
    }
    return std::strong_ordering::equal;
  }
};
template <size_t n>
StringLiteral(char const (&)[n]) -> StringLiteral<n - 1>;

}  // namespace Prong

#endif
