#pragma once

#include <ferrugo/core/predicates.hpp>
#include <ferrugo/seq/sequence.hpp>
#include <ferrugo/seq/slice.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct maybe_front_fn
{
    template <class T>
    auto operator()(const sequence<T>& s) const -> core::optional<T>
    {
        return s.get_next()();
    }
};

static const inline auto maybe_front = core::pipe(detail::maybe_front_fn{});

struct nth_fn
{
    auto operator()(std::ptrdiff_t index) const
    {
        return seq::drop(index) |= maybe_front;
    }
};

struct find_if_fn
{
    template <class Pred>
    auto operator()(Pred&& pred) const
    {
        return seq::drop_while(core::negate(std::forward<Pred>(pred))) |= maybe_front;
    }
};

}  // namespace detail

static const inline auto maybe_front = detail::maybe_front;
static const inline auto nth = detail::nth_fn{};
static const inline auto find_if = detail::find_if_fn{};
}  // namespace seq
}  // namespace ferrugo
