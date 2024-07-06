#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct drop_fn
{
    template <class In>
    struct next_function
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

    struct impl
    {
        std::ptrdiff_t m_count;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<T>
        {
            return sequence<T>{ next_function<T>{ m_count, s.get_next() } };
        }
    };

    auto operator()(std::ptrdiff_t count) const
    {
        return core::pipe(impl{ count });
    }
};

}  // namespace detail

static constexpr inline auto drop = detail::drop_fn{};
}  // namespace seq
}  // namespace ferrugo
