#pragma once

#include <ferrugo/seq/owning.hpp>
#include <ferrugo/seq/pipe.hpp>

namespace ferrugo
{

namespace seq
{
namespace detail
{

struct cached_fn
{
    template <class T>
    auto operator()(const sequence<T>& seq) const -> sequence<range_reference_t<std::vector<std::decay_t<T>>>>
    {
        std::vector<std::decay_t<T>> buffer;
        for (auto it = std::begin(seq); it != std::end(seq); ++it)
        {
            buffer.push_back(*it);
        }
        return owning(std::move(buffer));
    }
};

}  // namespace detail

static constexpr inline auto cached = pipe_t<detail::cached_fn>{ detail::cached_fn{} };

}  // namespace seq

}  // namespace ferrugo