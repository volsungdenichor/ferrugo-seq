#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct enumerate_fn
{
    template <class In>
    struct next_function
    {
        mutable std::ptrdiff_t m_index;
        next_function_t<In> m_next;

        auto operator()() const -> core::optional<std::tuple<std::ptrdiff_t, In>>
        {
            core::optional<In> n = m_next();
            if (!n)
            {
                return {};
            }
            return std::tuple<std::ptrdiff_t, In>{ m_index++, *n };
        }
    };

    struct impl
    {
        std::ptrdiff_t m_init;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<std::tuple<std::ptrdiff_t, T>>
        {
            return sequence<std::tuple<std::ptrdiff_t, T>>{ next_function<T>{ m_init, s.get_next() } };
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
