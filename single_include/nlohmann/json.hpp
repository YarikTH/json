#define NLOHMANN_JSON_VERSION_MAJOR 3
#define NLOHMANN_JSON_VERSION_MINOR 9
#define NLOHMANN_JSON_VERSION_PATCH 1

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include <cstdint>
#include <map>
#include <cassert>
#include <type_traits>

// C++ language standard detection
#if (defined(__cplusplus) && __cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    #define JSON_HAS_CPP_20
    #define JSON_HAS_CPP_17
    #define JSON_HAS_CPP_14
#elif (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_HAS_CXX17) && _HAS_CXX17 == 1) // fix for issue #464
    #define JSON_HAS_CPP_17
    #define JSON_HAS_CPP_14
#elif (defined(__cplusplus) && __cplusplus >= 201402L) || (defined(_HAS_CXX14) && _HAS_CXX14 == 1)
    #define JSON_HAS_CPP_14
#endif

// disable float-equal warnings on GCC/clang
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

// disable documentation warnings on clang
#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdocumentation"
#endif

// allow to disable exceptions
#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && !defined(JSON_NOEXCEPTION)
    #define JSON_THROW(exception) throw exception
    #define JSON_TRY try
    #define JSON_CATCH(exception) catch(exception)
    #define JSON_INTERNAL_CATCH(exception) catch(exception)
#else
    #include <cstdlib>
    #define JSON_THROW(exception) std::abort()
    #define JSON_TRY if(true)
    #define JSON_CATCH(exception) if(false)
    #define JSON_INTERNAL_CATCH(exception) if(false)
#endif

#define JSON_ASSERT(x) assert(x)

#define NLOHMANN_BASIC_JSON_TPL_DECLARATION                                \
    template<template<typename, typename, typename...> class ObjectType,   \
             template<typename, typename...> class ArrayType,              \
             class StringType, class BooleanType, class NumberIntegerType, \
             class NumberUnsignedType, class NumberFloatType,              \
             template<typename> class AllocatorType,                       \
             template<typename, typename = void> class JSONSerializer,     \
             class BinaryType>

#define NLOHMANN_BASIC_JSON_TPL                                            \
    basic_json<ObjectType, ArrayType, StringType, BooleanType,             \
    NumberIntegerType, NumberUnsignedType, NumberFloatType,                \
    AllocatorType, JSONSerializer, BinaryType>

#ifndef JSON_USE_IMPLICIT_CONVERSIONS
    #define JSON_USE_IMPLICIT_CONVERSIONS 1
#endif

#if JSON_USE_IMPLICIT_CONVERSIONS
    #define JSON_EXPLICIT
#else
    #define JSON_EXPLICIT explicit
#endif

namespace nlohmann
{
namespace detail
{
// alias templates to reduce boilerplate
template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<typename T>
using uncvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

template<typename T>
struct static_const
{
    static constexpr T value{};
};

template<typename T>
constexpr T static_const<T>::value;
}  // namespace detail
}  // namespace nlohmann

namespace nlohmann
{
namespace detail
{
template<typename ...Ts> struct make_void
{
    using type = void;
};
template<typename ...Ts> using void_t = typename make_void<Ts...>::type;
} // namespace detail
}  // namespace nlohmann


// https://en.cppreference.com/w/cpp/experimental/is_detected
namespace nlohmann
{
namespace detail
{
struct nonesuch
{
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    nonesuch(nonesuch const&&) = delete;
    void operator=(nonesuch const&) = delete;
    void operator=(nonesuch&&) = delete;
};

template<class Default,
         class AlwaysVoid,
         template<class...> class Op,
         class... Args>
struct detector
{
    using value_t = std::false_type;
    using type = Default;
};

// @5: note: in instantiation of template type alias 'get_template_function' requested here
// struct detector<Default, void_t<Op<Args...>>, Op, Args...>
//                                 ^
// @13: note: in instantiation of template type alias 'get_template_function' requested here
// struct detector<Default, void_t<Op<Args...>>, Op, Args...>
//                                 ^
// @21: note: in instantiation of template type alias 'from_json_function'
// requested here
// struct detector<Default, void_t<Op<Args...>>, Op, Args...>
//                                 ^
template<class Default, template<class...> class Op, class... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...>
{
    using value_t = std::true_type;
    using type = Op<Args...>;
};

// @3: note: in instantiation of template class 'nlohmann::detail::detector<
// nlohmann::detail::nonesuch, void, get_template_function, const nlohmann::ordered_json&, nlohmann::ordered_json>  >'
//  requested here
// using is_detected =
// ^
// @4: note: during template argument deduction for class template partial specialization 'detector<Default, void_t<Op<Args...>>, Op, Args...>'
// [with Default = nlohmann::detail::nonesuch, Op = get_template_function, Args = <const nlohmann::ordered_json&, nlohmann::ordered_json>]
// @11: note: in instantiation of template class 'nlohmann::detail::detector<
// nlohmann::detail::nonesuch, void, get_template_function, const nlohmann::ordered_json &, const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>>*>'
// requested here
// using is_detected =
// ^
// @12: note: during template argument deduction for class template partial specialization 'detector<Default, void_t<Op<Args...>>, Op, Args...>'
// [with Default = nlohmann::detail::nonesuch, Op = get_template_function
//     , Args = <const nlohmann::ordered_json &, const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>> *>]
template<template<class...> class Op, class... Args>
using is_detected =
    typename detector<nonesuch, void, Op, Args...>::value_t;

// @19: note: in instantiation of template class 'nlohmann::detail::detector<
// nlohmann::detail::nonesuch, void, from_json_function, nlohmann::adl_serializer<const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>> *, void>, const nlohmann::ordered_json &, const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>> *&>'
// requested here
// using detected_t = typename detector<nonesuch, void, Op, Args...>::type;
// ^
// @20: note: during template argument deduction for class template partial specialization 'detector<
// Default, void_t<Op<Args...>>, Op, Args...>'
// [with Default = nlohmann::detail::nonesuch, Op = from_json_function
//     , Args = <nlohmann::adl_serializer<const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>> *, void>, const nlohmann::ordered_json &, const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>> *&>]
template<template<class...> class Op, class... Args>
using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

// @18: note: in instantiation of template type alias 'detected_t' requested here
// using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;
// ^
template<class Expected, template<class...> class Op, class... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

}  // namespace detail
}  // namespace nlohmann

namespace nlohmann
{
template<typename T = void, typename SFINAE = void>
struct adl_serializer;

template<template<typename U, typename V, typename... Args> class ObjectType =
         std::map,
         template<typename U, typename... Args> class ArrayType = std::vector,
         class StringType = std::string, class BooleanType = bool,
         class NumberIntegerType = std::int64_t,
         class NumberUnsignedType = std::uint64_t,
         class NumberFloatType = double,
         template<typename U> class AllocatorType = std::allocator,
         template<typename T, typename SFINAE = void> class JSONSerializer =
         adl_serializer,
         class BinaryType = std::vector<std::uint8_t>>
class basic_json;

template<class Key, class T, class IgnoredLess, class Allocator>
struct ordered_map;

using ordered_json = basic_json<nlohmann::ordered_map>;

}  // namespace nlohmann

namespace nlohmann
{
namespace detail
{
template<typename> struct is_basic_json : std::false_type {};

NLOHMANN_BASIC_JSON_TPL_DECLARATION
struct is_basic_json<NLOHMANN_BASIC_JSON_TPL> : std::true_type {};

// @22: note: while substituting deduced template arguments into function template 'from_json'
// [with BasicJsonType = const nlohmann::ordered_json &, ValueType = const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>> *]
// using from_json_function = decltype(T::from_json(std::declval<Args>()...));
//                                     ^
template<typename T, typename... Args>
using from_json_function = decltype(T::from_json(std::declval<Args>()...));

// @6: note: while substituting deduced template arguments into function template 'get'
// [with ValueTypeCV = const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>>*
//     , ValueType = (no value)
//     , $2 = (no value)]
// using get_template_function = decltype(std::declval<T>().template get<U>());
//                                                                   ^
// @14: note: while substituting deduced template arguments into function template 'get'
// [with ValueTypeCV = const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>> *
//     , ValueType = (no value)
//     , $2 = (no value)]
// using get_template_function = decltype(std::declval<T>().template get<U>());
//                                                                   ^
template<typename T, typename U>
using get_template_function = decltype(std::declval<T>().template get<U>());

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Without this detection code, sample compiles
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// trait checking if JSONSerializer<T>::from_json(json const&, udt&) exists
template<typename BasicJsonType, typename T, typename = void>
struct has_from_json : std::false_type {};

template<typename BasicJsonType, typename T>
struct has_from_json < BasicJsonType, T,
           enable_if_t < !is_basic_json<T>::value >>
{
    using serializer = typename BasicJsonType::template json_serializer<T, void>;

    static constexpr bool value =
    // @17: note: in instantiation of template type alias 'is_detected_exact'
    // requested here
    //         is_detected_exact<void, from_json_function, serializer,
    //         ^
        is_detected_exact<void, from_json_function, serializer,
        const BasicJsonType&, T&>::value;
};
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

}  // namespace detail
}  // namespace nlohmann

namespace nlohmann
{
namespace detail
{
enum class value_t : std::uint8_t
{
    null,             ///< null value
};

}  // namespace detail
}  // namespace nlohmann


namespace nlohmann
{
namespace detail
{

struct from_json_fn
{
    // @24: note: in instantiation of template class 'std::pair<const std::string, nlohmann::ordered_json>'
    // requested here
    //     -> decltype(from_json(j, val), void())
    //                 ^
    template<typename BasicJsonType, typename T>
    auto operator()(const BasicJsonType& j, T& val) const
    noexcept(noexcept(from_json(j, val)))
    -> decltype(from_json(j, val), void())
    {
        return from_json(j, val);
    }
};
}  // namespace detail

/// namespace to hold default `from_json` function
/// to see why this is required:
/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html
namespace
{
constexpr const auto& from_json = detail::static_const<detail::from_json_fn>::value;
} // namespace

template<typename, typename>
struct adl_serializer
{
    // @23: note: while substituting deduced template arguments into function template 'operator()'
    // [with BasicJsonType = nlohmann::ordered_json
    //     , T = const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>> *]
    //     -> decltype(::nlohmann::from_json(std::forward<BasicJsonType>(j), val), void())
    //                 ^
    template<typename BasicJsonType, typename ValueType>
    static auto from_json(BasicJsonType&& j, ValueType& val) noexcept(
        noexcept(::nlohmann::from_json(std::forward<BasicJsonType>(j), val)))
    -> decltype(::nlohmann::from_json(std::forward<BasicJsonType>(j), val), void())
    {
        ::nlohmann::from_json(std::forward<BasicJsonType>(j), val);
    }
};

}  // namespace nlohmann

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
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Without this code, sample compiles
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
    // @16: note: in instantiation of static data member 'nlohmann::detail::has_from_json<
    // nlohmann::ordered_json, const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>> *, void>::value'
    // requested here
    //                    detail::has_from_json<basic_json_t, ValueType>::value,
    //                                                                    ^
                   detail::has_from_json<basic_json_t, ValueType>::value,
                   int > = 0 >
    // @15: note: while substituting prior template arguments into non-type template parameter
    // [with ValueTypeCV = const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>> *
    //     , ValueType = const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>> *]
    //     ValueType get() const noexcept(noexcept(
    //               ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
    constexpr auto get_ptr() const noexcept
        // @8: note: while substituting deduced template arguments into function template 'operator type-parameter-0-0'
        // [with ValueType = const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>>*
        //     , $1 = (no value)]
        //      -> decltype(std::declval<const basic_json_t&>().get_impl_ptr(std::declval<PointerType>()))
        //                                                                   ^
        -> decltype(std::declval<const basic_json_t&>().get_impl_ptr(std::declval<PointerType>()))
    {
        return static_cast<const object_t*>(nullptr);
    }

    // @7: note: while substituting prior template arguments into non-type template parameter
    // [with ValueTypeCV = const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>>*
    //     , ValueType = const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>>*]
    //  ValueType get() const noexcept(noexcept(
    //            ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
    // @2: note: in instantiation of template type alias 'is_detected' requested here
    // && detail::is_detected<detail::get_template_function, const basic_json_t&, ValueType>::value
    //            ^
    // @10: note: in instantiation of template type alias 'is_detected' requested here
    // && detail::is_detected<detail::get_template_function, const basic_json_t&, ValueType>::value
    //            ^
                   && detail::is_detected<detail::get_template_function, const basic_json_t&, ValueType>::value
                   , int >::type = 0 >
    // @1: note: while substituting prior template arguments into non-type template parameter
    // [with ValueType = nlohmann::ordered_json]
    //     JSON_EXPLICIT operator ValueType() const
    //                   ^~~~~~~~~~~~~~~~~~~~~~~~~~
    // @9: note: while substituting prior template arguments into non-type template parameter
    // [with ValueType = const nlohmann::ordered_map<std::string, nlohmann::ordered_json, std::less<void>, std::allocator<std::pair<const std::string, nlohmann::ordered_json>>>*]
    //     JSON_EXPLICIT operator ValueType() const
    //                   ^~~~~~~~~~~~~~~~~~~~~~~~~~
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

// restore GCC/clang diagnostic settings
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic pop
#endif
#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif

// clean up
#undef JSON_ASSERT
#undef JSON_INTERNAL_CATCH
#undef JSON_CATCH
#undef JSON_THROW
#undef JSON_TRY
#undef JSON_HAS_CPP_14
#undef JSON_HAS_CPP_17
#undef NLOHMANN_BASIC_JSON_TPL_DECLARATION
#undef NLOHMANN_BASIC_JSON_TPL
#undef JSON_EXPLICIT





using J = nlohmann::ordered_json;

#if USING_MAGIC
using magic = decltype((std::declval<J&>() = std::declval<const J&>()));
#endif

#if USING_MAGIC_2
constexpr bool magic_2 = std::is_copy_constructible<J>::value;
#endif

constexpr bool foo = std::is_copy_assignable<J>::value;

int main()
{
}
