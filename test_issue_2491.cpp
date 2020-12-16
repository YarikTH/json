#include <type_traits>
#include "nlohmann/json.hpp"
#include "nlohmann/ordered_map.hpp"

using J = nlohmann::ordered_json;

#if MAGIC
    constexpr bool bar = std::is_copy_constructible<J>::value;
#endif

constexpr bool foo = std::is_copy_assignable<J>::value;
