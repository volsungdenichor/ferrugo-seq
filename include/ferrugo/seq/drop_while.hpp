#pragma once

#include <ferrugo/seq/invoke.hpp>
#include <ferrugo/seq/pipe.hpp>
#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct drop_while_fn
{
    template <class Pred, class In>
    struct next_function
    {
        Pred m_pred;
        next_function_t<In> m_next;
        mutable bool m_init = true;

        auto operator()() const -> maybe<In>
        {
            if (m_init)
            {
                while (true)
                {
                    maybe<In> res = m_next();
                    if (!res)
                    {
                        return {};
                    }
                    if (!invoke(m_pred, *res))
                    {
                        m_init = false;
                        return res;
                    }
                }
            }
            return m_next();
        }
    };

    template <class Pred>
    struct impl_t
    {
        Pred m_func;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<T>
        {
            return sequence<T>{ next_function<Pred, T>{ m_func, s.get_next() } };
        }
    };

    template <class Pred>
    auto operator()(Pred&& pred) const -> pipe_t<impl_t<std::decay_t<Pred>>>
    {
        return { { std::forward<Pred>(pred) } };
    }
};

struct drop_while_i_fn
{
    template <class Pred, class In>
    struct next_function
    {
        Pred m_pred;
        next_function_t<In> m_next;
        mutable std::ptrdiff_t m_index = 0;
        mutable bool m_init = true;

        auto operator()() const -> maybe<In>
        {
            if (m_init)
            {
                while (true)
                {
                    maybe<In> res = m_next();
                    if (!res)
                    {
                        return {};
                    }
                    if (!invoke(m_pred, concat(m_index++, *res)))
                    {
                        m_init = false;
                        return res;
                    }
                }
            }
            return m_next();
        }
    };

    template <class Pred>
    struct impl_t
    {
        Pred m_func;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<T>
        {
            return sequence<T>{ next_function<Pred, T>{ m_func, s.get_next() } };
        }
    };

    template <class Pred>
    auto operator()(Pred&& pred) const -> pipe_t<impl_t<std::decay_t<Pred>>>
    {
        return { { std::forward<Pred>(pred) } };
    }
};

}  // namespace detail

static constexpr inline auto drop_while = detail::drop_while_fn{};
static constexpr inline auto drop_while_i = detail::drop_while_i_fn{};

}  // namespace seq
}  // namespace ferrugo