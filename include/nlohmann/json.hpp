#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#define INCLUDE_NLOHMANN_JSON_HPP_

#define NLOHMANN_JSON_VERSION_MAJOR 3
#define NLOHMANN_JSON_VERSION_MINOR 9
#define NLOHMANN_JSON_VERSION_PATCH 1

#include <algorithm>
#include <cstddef>
#include <iosfwd>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/detail/meta/cpp_future.hpp>
#include <nlohmann/detail/meta/type_traits.hpp>
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/ordered_map.hpp>

namespace nlohmann
{

NLOHMANN_BASIC_JSON_TPL_DECLARATION
class basic_json
{
  private:
    /// workaround type for MSVC
    using basic_json_t = NLOHMANN_BASIC_JSON_TPL;
  public:
    using value_t = detail::value_t;
    template<typename T, typename SFINAE>
    using json_serializer = JSONSerializer<T, SFINAE>;

    /////////////////////
    // container types //
    /////////////////////

    /// the type of elements in a basic_json container
    using value_type = basic_json;

    /// the allocator type
    using allocator_type = AllocatorType<basic_json>;

    /// the type of an element pointer
    using pointer = typename std::allocator_traits<allocator_type>::pointer;
    /// @}

#if defined(JSON_HAS_CPP_14)
    // Use transparent comparator if possible, combined with perfect forwarding
    // on find() and count() calls prevents unnecessary string construction.
    using object_comparator_t = std::less<>;
#else
    using object_comparator_t = std::less<StringType>;
#endif
    using object_t = ObjectType<StringType,
          basic_json,
          object_comparator_t,
          AllocatorType<std::pair<const StringType,
          basic_json>>>;
  private:
    union json_value
    {
        /// object (stored with pointer to save storage)
        object_t* object;
    };
  public:
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    basic_json& operator=(basic_json other) noexcept (
        std::is_nothrow_move_constructible<value_t>::value&&
        std::is_nothrow_move_assignable<value_t>::value&&
        std::is_nothrow_move_constructible<json_value>::value&&
        std::is_nothrow_move_assignable<json_value>::value
    )
    {
        return *this;
    }

    template < typename ValueTypeCV, typename ValueType = detail::uncvref_t<ValueTypeCV>,
               detail::enable_if_t <
                   !detail::is_basic_json<ValueType>::value &&
                   detail::has_from_json<basic_json_t, ValueType>::value,
                   int > = 0 >
    ValueType get() const noexcept(noexcept(
                                       JSONSerializer<ValueType>::from_json(std::declval<const basic_json_t&>(), std::declval<ValueType&>())))
    {
        return ValueType();
    }

    constexpr const object_t* get_impl_ptr(const object_t* /*unused*/) const noexcept
    {
        return nullptr;
    }

    template < typename PointerType, typename std::enable_if <
                   std::is_pointer<PointerType>::value&&
                   std::is_const<typename std::remove_pointer<PointerType>::type>::value, int >::type = 0 >
    constexpr auto get_ptr() const noexcept -> decltype(std::declval<const basic_json_t&>().get_impl_ptr(std::declval<PointerType>()))
    {
        return static_cast<const object_t*>(nullptr);
    }

    template<typename PointerType, typename std::enable_if<
                 std::is_pointer<PointerType>::value, int>::type = 0>
    constexpr auto get() const noexcept -> decltype(std::declval<const basic_json_t&>().template get_ptr<PointerType>())
    {
        return PointerType();
    }

    template < typename ValueType, typename std::enable_if <
                   !std::is_pointer<ValueType>::value&&
                   !detail::is_basic_json<ValueType>::value
#if defined(JSON_HAS_CPP_17) && (defined(__GNUC__) || (defined(_MSC_VER) && _MSC_VER >= 1910 && _MSC_VER <= 1914))
                   && !std::is_same<ValueType, typename std::string_view>::value
#endif
                   && detail::is_detected<detail::get_template_function, const basic_json_t&, ValueType>::value
                   , int >::type = 0 >
    JSON_EXPLICIT operator ValueType() const
    {
        return ValueType();
    }
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  private:
    /// the value of the current element
    json_value m_value = {};
};
} // namespace nlohmann

#include <nlohmann/detail/macro_unscope.hpp>

#endif  // INCLUDE_NLOHMANN_JSON_HPP_
