#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct chain_fn
{
    template <class In>
    struct next_function
    {
        next_function_t<In> m_first;
        next_function_t<In> m_second;
        mutable bool m_first_finished = false;

        auto operator()() const -> maybe<In>
        {
            if (!m_first_finished)
            {
                maybe<In> n = m_first();
                if (n)
                {
                    return n;
                }
                else
                {
                    m_first_finished = true;
                }
            }
            return m_second();
        }
    };

    template <class T>
    auto operator()(const sequence<T>& lhs, const sequence<T>& rhs) const -> sequence<T>
    {
        return sequence<T>{ next_function<T>{ lhs.get_next(), rhs.get_next() } };
    }
};

}  // namespace detail

static constexpr inline auto chain = detail::chain_fn{};

}  // namespace seq
}  // namespace ferrugo
