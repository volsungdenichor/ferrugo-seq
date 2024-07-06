#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct step_fn
{
    template <class In>
    struct next_function
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

static constexpr inline auto step = detail::step_fn{};
}  // namespace seq
}  // namespace ferrugo
