#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

template <class Pred, class In>
struct drop_while
{
    Pred m_pred;
    next_function_t<In> m_next;
    mutable bool m_init = true;

    auto operator()() const -> core::optional<In>
    {
        if (m_init)
        {
            while (true)
            {
                core::optional<In> res = m_next();
                if (!res)
                {
                    return {};
                }
                if (!std::invoke(m_pred, *res))
                {
                    m_init = false;
                    return res;
                }
            }
        }
        return m_next();
    }
};

template <class Pred, class In>
struct take_while
{
    Pred m_pred;
    next_function_t<In> m_next;

    auto operator()() const -> core::optional<In>
    {
        core::optional<In> res = m_next();
        if (!(res && std::invoke(m_pred, *res)))
        {
            return {};
        }
        return res;
    }
};

template <template <class, class> class Impl>
struct conditional_slice_fn
{
    template <class Pred>
    struct impl
    {
        Pred m_pred;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<T>
        {
            return sequence<T>{ Impl<Pred, T>{ m_pred, s.get_next() } };
        }
    };

    template <class Pred>
    auto operator()(Pred&& pred) const
    {
        return core::pipe(impl<std::decay_t<Pred>>{ std::forward<Pred>(pred) });
    }
};

}  // namespace detail
static constexpr inline auto drop_while = detail::conditional_slice_fn<detail::drop_while>{};
static constexpr inline auto take_while = detail::conditional_slice_fn<detail::take_while>{};
}  // namespace seq
}  // namespace ferrugo