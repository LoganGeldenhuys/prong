#ifndef PRONG_SUBSTITUTION_VECTOR
#define PRONG_SUBSTITUTION_VECTOR

#include <vector>
#include "substitution.h"

namespace Prong {
// Template class just for type aliasing, doesn't hold any data
template <class... Args>
struct SubstitutionsVector {
  using type = std::vector<std::tuple<Args...>>;
};

// Type alias for convenient usage without needing to create tuples
template <Prong::StringLiteral... Args>
using SubstitutionVector =
    typename SubstitutionsVector<Prong::Substitution<Args>...>::type;
}  // namespace Prong

#endif  // PRONG_SUBSTITUTION_VECTOR
