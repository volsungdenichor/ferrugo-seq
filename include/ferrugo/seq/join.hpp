#pragma once

#include <ferrugo/seq/pipe.hpp>
#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct join_fn
{
    template <class In>
    struct next_function
    {
        next_function_t<sequence<In>> m_next;
        mutable next_function_t<In> m_sub = {};

        auto operator()() const -> maybe<In>
        {
            while (true)
            {
                if (!m_sub)
                {
                    maybe<sequence<In>> next = m_next();
                    if (!next)
                    {
                        return {};
                    }
                    m_sub = next->get_next();
                    continue;
                }
                maybe<In> next_sub = m_sub();
                if (next_sub)
                {
                    return next_sub;
                }
                else
                {
                    m_sub = {};
                    continue;
                }
            }
            return {};
        }
    };

    template <class T>
    auto operator()(const sequence<sequence<T>>& s) const -> sequence<T>
    {
        return sequence<T>{ next_function<T>{ s.get_next() } };
    }
};

}  // namespace detail

static constexpr inline auto join = pipe_t<detail::join_fn>{ detail::join_fn{} };

}  // namespace seq
}  // namespace ferrugo
