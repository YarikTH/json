#pragma once

#include <functional> // less
#include <memory> // allocator
#include <utility> // pair
#include <vector> // vector

namespace nlohmann
{

/// ordered_map: a minimal map-like container that preserves insertion order
/// for use within nlohmann::basic_json<ordered_map>
template <class Key, class T, class IgnoredLess = std::less<Key>,
          class Allocator = std::allocator<std::pair<const Key, T>>>
                  struct ordered_map : std::vector<std::pair<const Key, T>, Allocator>
{
    using Container = std::vector<std::pair<const Key, T>, Allocator>;

    // Explicit constructors instead of `using Container::Container`
    // otherwise older compilers choke on it (GCC <= 5.5, xcode <= 9.4)
    ordered_map(const Allocator& alloc = Allocator()) : Container{alloc} {}
};

}  // namespace nlohmann
