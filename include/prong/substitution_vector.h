#ifndef PRONG_SUBSTITUTION_VECTOR
#define PRONG_SUBSTITUTION_VECTOR

#include <vector>
#include "substitution.h"

namespace Prong {
// Template class just for type aliasing, doesn't hold any data
template <StringLiteral... keys>
struct SubstitutionsVector {
  using type = std::vector<std::tuple<Substitution<keys>...>>;
};

// Type alias for convenient usage without needing to create tuples
template <StringLiteral... keys>
using SubstitutionVector =
    typename SubstitutionsVector<keys...>::type;
}  // namespace Prong

#endif  // PRONG_SUBSTITUTION_VECTOR
