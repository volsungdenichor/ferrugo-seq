#pragma once

#include <ferrugo/seq/numeric.hpp>
#include <ferrugo/seq/sequence.hpp>
#include <ferrugo/seq/zip.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct enumerate_fn
{
    struct impl
    {
        std::ptrdiff_t m_init;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<std::tuple<std::ptrdiff_t, T>>
        {
            return seq::zip(seq::iota(m_init), s);
        }
    };

    auto operator()(std::ptrdiff_t init) const
    {
        return core::pipe(impl{ init });
    }
};

}  // namespace detail

static constexpr inline auto enumerate = detail::enumerate_fn{};
}  // namespace seq
}  // namespace ferrugo
