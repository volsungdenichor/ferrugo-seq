#pragma once

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

    template <class Pred>
    struct impl
    {
        Pred m_pred;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<T>
        {
            return sequence<T>{ next_function<Pred, T>{ m_pred, s.get_next() } };
        }
    };

    template <class Pred>
    auto operator()(Pred&& pred) const
    {
        return core::pipe(impl<std::decay_t<Pred>>{ std::forward<Pred>(pred) });
    }
};

}  // namespace detail
static constexpr inline auto take_while = detail::take_while_fn{};
}  // namespace seq
}  // namespace ferrugo