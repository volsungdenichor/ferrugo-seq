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

template <class...>
struct concat_result;

template <class L, class R>
struct concat_result<L, R>
{
    using type = tuple<L, R>;
};

template <class... Args>
using concat_result_t = typename concat_result<Args...>::type;

template <class... L, class... R>
struct concat_result<tuple<L...>, tuple<R...>>
{
    using type = tuple<L..., R...>;
};

template <class L, class... R>
struct concat_result<L, tuple<R...>>
{
    using type = tuple<L, R...>;
};

template <class... L, class R>
struct concat_result<tuple<L...>, R>
{
    using type = tuple<L..., R>;
};

template <class... L, class... R>
constexpr auto concat(tuple<L...> lhs, tuple<R...> rhs) -> concat_result_t<tuple<L...>, tuple<R...>>
{
    using lhs_t = typename tuple<L...>::base_t;
    using rhs_t = typename tuple<R...>::base_t;
    return std::tuple_cat(std::move(static_cast<lhs_t&>(lhs)), std::move(static_cast<rhs_t&>(rhs)));
}

template <class L, class... R, std::enable_if_t<!is_tuple<std::decay_t<L>>::value, int> = 0>
constexpr auto concat(L&& lhs, tuple<R...> rhs) -> concat_result_t<L, tuple<R...>>
{
    return concat(tuple<L>{ std::forward<L>(lhs) }, std::move(rhs));
}

template <class... L, class R, std::enable_if_t<!is_tuple<std::decay_t<R>>::value, int> = 0>
constexpr auto concat(tuple<L...> lhs, R&& rhs) -> concat_result_t<tuple<L...>, R>
{
    return concat(std::move(lhs), tuple<R>{ std::forward<R>(rhs) });
}

template <
    class L,
    class R,
    std::enable_if_t<!is_tuple<std::decay_t<L>>::value && !is_tuple<std::decay_t<R>>::value, int> = 0>
constexpr auto concat(L&& lhs, R&& rhs) -> concat_result_t<L, R>
{
    return tuple<L, R>{ std::forward<L>(lhs), std::forward<R>(rhs) };
}

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
