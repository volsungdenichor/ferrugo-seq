#pragma once

#include <algorithm>
#include <ferrugo/seq/owning.hpp>
#include <ferrugo/seq/pipe.hpp>

namespace ferrugo
{

namespace seq
{
namespace detail
{

struct sort_fn
{
    template <class Compare>
    struct impl_t
    {
        Compare m_compare;

        template <class T>
        auto operator()(const sequence<T>& seq) const -> sequence<range_reference_t<std::vector<std::decay_t<T>>>>
        {
            std::vector<std::decay_t<T>> buffer;
            for (auto it = std::begin(seq); it != std::end(seq); ++it)
            {
                buffer.push_back(*it);
            }
            std::sort(std::begin(buffer), std::end(buffer), m_compare);
            return owning(std::move(buffer));
        }
    };

    template <class Compare = std::less<>>
    auto operator()(Compare&& compare = {}) const -> pipe_t<impl_t<std::decay_t<Compare>>>
    {
        return { { std::forward<Compare>(compare) } };
    }
};

}  // namespace detail

static constexpr inline auto sort = detail::sort_fn{};

}  // namespace seq

}  // namespace ferrugo
