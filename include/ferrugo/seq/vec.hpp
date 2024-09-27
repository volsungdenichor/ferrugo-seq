#pragma once

#include <ferrugo/seq/owning.hpp>
#include <vector>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct vec_fn
{
    template <class T, class... Tail>
    auto operator()(T head, Tail&&... tail) const -> sequence<const T&>
    {
        return owning(std::vector<T>{ std::move(head), std::forward<Tail>(tail)... });
    }
};

}  // namespace detail

static constexpr inline auto vec = detail::vec_fn{};

}  // namespace seq
}  // namespace ferrugo
