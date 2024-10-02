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
    auto operator()(const sequence<T>& seq) const
    {
        std::vector<T> buffer;
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