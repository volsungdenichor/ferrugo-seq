#pragma once

#include <tuple>

namespace ferrugo
{
namespace seq
{

template <class... Types>
struct tuple : public std::tuple<Types...>
{
    using base_t = std::tuple<Types...>;
    using base_t::base_t;

    template <class... Args, class = std::enable_if_t<std::is_constructible_v<base_t, Args...>>>
    tuple(Args&&... args) : base_t(std::forward<Args>(args)...)
    {
    }
};

template <class... Types>
tuple(Types&&...) -> tuple<std::decay_t<Types>...>;

template <std::size_t I, class... Types>
constexpr auto get(const tuple<Types...>& item) -> const std::tuple_element_t<I, std::tuple<Types...>>&
{
    return std::get<I>(static_cast<const typename tuple<Types...>::base_t&>(item));
}

template <std::size_t I, class... Types>
constexpr auto get(tuple<Types...>& item) -> std::tuple_element_t<I, std::tuple<Types...>>&
{
    return std::get<I>(static_cast<typename tuple<Types...>::base_t&>(item));
}

template <std::size_t I, class... Types>
constexpr auto get(tuple<Types...>&& item) -> std::tuple_element_t<I, std::tuple<Types...>>&&
{
    return std::get<I>(static_cast<typename tuple<Types...>::base_t&&>(item));
}

template <class T>
struct is_tuple : std::false_type
{
};

template <class... Types>
struct is_tuple<tuple<Types...>> : std::true_type
{
};

struct tuplify_fn
{
    template <class... Types>
    static constexpr auto from_std_tuple(std::tuple<Types...> arg) -> tuple<Types...>
    {
        return { std::move(arg) };
    }

    template <class... Types>
    static constexpr auto to_std_tuple(tuple<Types...> arg) -> std::tuple<Types...>
    {
        return static_cast<std::tuple<Types...>>(arg);
    }

    template <class T>
    static constexpr auto to_std_tuple(T&& arg) -> std::tuple<T>
    {
        return { std::forward<T>(arg) };
    }

    template <class... Args>
    constexpr auto operator()(Args&&... args) const
    {
        return from_std_tuple(std::tuple_cat(to_std_tuple(std::forward<Args>(args))...));
    }
};

static constexpr inline auto tuplify = tuplify_fn{};

template <class... Args>
using tuplify_result_t = decltype(tuplify(std::declval<Args>()...));

}  // namespace seq
}  // namespace ferrugo

namespace std
{
template <class... Types>
struct tuple_size<ferrugo::seq::tuple<Types...>> : tuple_size<tuple<Types...>>
{
};

template <size_t I, class... Types>
struct tuple_element<I, ::ferrugo::seq::tuple<Types...>> : tuple_element<I, std::tuple<Types...>>
{
};

}  // namespace std
