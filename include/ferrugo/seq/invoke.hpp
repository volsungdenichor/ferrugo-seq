#pragma once

#include <ferrugo/seq/tuple.hpp>
#include <functional>

namespace ferrugo
{
namespace seq
{

template <class Func, class Head, class... Tail, std::enable_if_t<!is_tuple<std::decay_t<Head>>::value, int> = 0>
constexpr auto invoke(Func&& func, Head&& head, Tail&&... tail) -> std::invoke_result_t<Func, Head, Tail...>
{
    return std::invoke(std::forward<Func>(func), std::forward<Head>(head), std::forward<Tail>(tail)...);
}

template <class Func, class Arg, std::enable_if_t<is_tuple<std::decay_t<Arg>>::value, int> = 0>
constexpr auto invoke(Func&& func, Arg&& arg) -> decltype(std::apply(std::forward<Func>(func), std::forward<Arg>(arg)))
{
    return std::apply(std::forward<Func>(func), std::forward<Arg>(arg));
}

template <class Func, class... Args>
using invoke_result_t = decltype(invoke(std::declval<Func>(), std::declval<Args>()...));

}  // namespace seq
}  // namespace ferrugo