#pragma once

#include <ferrugo/seq/tuple.hpp>
#include <functional>

namespace ferrugo
{
namespace seq
{

namespace detail
{

struct do_not_deconstruct_fn
{
    template <class Func>
    struct impl_t
    {
        Func m_func;

        template <class... Args>
        constexpr auto operator()(Args&&... args) const -> std::invoke_result_t<Func, Args...>
        {
            return std::invoke(m_func, std::forward<Args>(args)...);
        }
    };

    template <class Func>
    auto operator()(Func&& func) const -> impl_t<std::decay_t<Func>>
    {
        return { std::forward<Func>(func) };
    }
};

}  // namespace detail

static constexpr inline auto do_not_deconstruct = detail::do_not_deconstruct_fn{};

template <class Func>
struct should_deconstruct : std::true_type
{
};

template <class Func>
struct should_deconstruct<detail::do_not_deconstruct_fn::impl_t<Func>> : std::false_type
{
};

template <class Func, class Head, class... Tail, std::enable_if_t<!is_tuple<std::decay_t<Head>>{}, int> = 0>
constexpr auto invoke(Func&& func, Head&& head, Tail&&... tail) -> std::invoke_result_t<Func, Head, Tail...>
{
    return std::invoke(std::forward<Func>(func), std::forward<Head>(head), std::forward<Tail>(tail)...);
}

template <
    class Func,
    class Arg,
    std::enable_if_t<should_deconstruct<std::decay_t<Func>>{} && is_tuple<std::decay_t<Arg>>{}, int> = 0>
constexpr auto invoke(Func&& func, Arg&& arg) -> decltype(std::apply(std::forward<Func>(func), std::forward<Arg>(arg)))
{
    return std::apply(std::forward<Func>(func), std::forward<Arg>(arg));
}

template <
    class Func,
    class Arg,
    std::enable_if_t<!should_deconstruct<std::decay_t<Func>>{} && is_tuple<std::decay_t<Arg>>{}, int> = 0>
constexpr auto invoke(Func&& func, Arg&& arg) -> decltype(std::invoke(std::forward<Func>(func), std::forward<Arg>(arg)))
{
    return std::invoke(std::forward<Func>(func), std::forward<Arg>(arg));
}

template <class Func, class... Args>
using invoke_result_t = decltype(invoke(std::declval<Func>(), std::declval<Args>()...));

}  // namespace seq
}  // namespace ferrugo