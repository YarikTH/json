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
    using key_type = Key;
    using mapped_type = T;
    using Container = std::vector<std::pair<const Key, T>, Allocator>;
    using typename Container::iterator;
    using typename Container::const_iterator;
    using typename Container::size_type;
    using typename Container::value_type;

    // Explicit constructors instead of `using Container::Container`
    // otherwise older compilers choke on it (GCC <= 5.5, xcode <= 9.4)
    ordered_map(const Allocator& alloc = Allocator()) : Container{alloc} {}
    template <class It>
    ordered_map(It first, It last, const Allocator& alloc = Allocator())
        : Container{first, last, alloc} {}
    ordered_map(std::initializer_list<T> init, const Allocator& alloc = Allocator() )
        : Container{init, alloc} {}
};

}  // namespace nlohmann
