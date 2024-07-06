#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

template <class In>
struct drop
{
    mutable std::ptrdiff_t m_count;
    next_function_t<In> m_next;

    auto operator()() const -> core::optional<In>
    {
        while (m_count-- > 0)
        {
            m_next();
        }
        return m_next();
    }
};

template <class In>
struct take
{
    mutable std::ptrdiff_t m_count;
    next_function_t<In> m_next;

    auto operator()() const -> core::optional<In>
    {
        if (m_count-- > 0)
        {
            return m_next();
        }
        return {};
    }
};

template <class In>
struct step
{
    mutable std::ptrdiff_t m_count;
    next_function_t<In> m_next;
    mutable std::ptrdiff_t m_index = 0;

    auto operator()() const -> core::optional<In>
    {
        while (true)
        {
            core::optional<In> n = m_next();
            if (!n)
            {
                break;
            }
            if (m_index++ % m_count == 0)
            {
                return n;
            }
        }
        return {};
    }
};

template <template <class> class Impl>
struct slice_fn
{
    struct impl
    {
        std::ptrdiff_t m_count;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<T>
        {
            return sequence<T>{ Impl<T>{ m_count, s.get_next() } };
        }
    };

    auto operator()(std::ptrdiff_t count) const
    {
        return core::pipe(impl{ count });
    }
};

}  // namespace detail

static constexpr inline auto drop = detail::slice_fn<detail::drop>{};
static constexpr inline auto take = detail::slice_fn<detail::take>{};
static constexpr inline auto step = detail::slice_fn<detail::step>{};
}  // namespace seq
}  // namespace ferrugo
