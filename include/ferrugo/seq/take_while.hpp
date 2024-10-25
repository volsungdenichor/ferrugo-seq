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

struct take_while_fn
{
    template <class Pred, class In>
    struct next_function
    {
        Pred m_pred;
        next_function_t<In> m_next;

        auto operator()() const -> maybe<In>
        {
            maybe<In> res = m_next();
            if (!(res && invoke(m_pred, *res)))
            {
                return {};
            }
            return res;
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

struct take_while_i_fn
{
    template <class Pred, class In>
    struct next_function
    {
        Pred m_pred;
        next_function_t<In> m_next;
        mutable std::ptrdiff_t m_index = 0;

        auto operator()() const -> maybe<In>
        {
            maybe<In> res = m_next();
            if (!(res && invoke(m_pred, tuplify(m_index++, *res))))
            {
                return {};
            }
            return res;
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

static constexpr inline auto take_while = detail::take_while_fn{};
static constexpr inline auto take_while_i = detail::take_while_i_fn{};

}  // namespace seq
}  // namespace ferrugo