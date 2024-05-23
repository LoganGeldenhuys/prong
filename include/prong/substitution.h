#ifndef PRONG_SUBSTITUTION_H
#define PRONG_SUBSTITUTION_H

#include <string>
#include "string_literal.h"

namespace Prong {

// The input type for all prompt types. Interacts with underlying
// TemplateStrings of prompts at compile time to ensure there is no key mismatch
template <StringLiteral key>
class Substitution {
 public:
  static constexpr const char* my_key_c = key;
  static constexpr StringLiteral<key.size> my_key = key;
  std::string value;

  Substitution(std::string value) : value(value){};
};

}  // namespace Prong

#endif
