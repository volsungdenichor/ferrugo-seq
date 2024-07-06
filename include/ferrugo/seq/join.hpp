#pragma once

#include <ferrugo/seq/sequence.hpp>
#include <ferrugo/seq/transform.hpp>

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

        auto operator()() const -> core::optional<In>
        {
            while (true)
            {
                if (!m_sub)
                {
                    core::optional<sequence<In>> next = m_next();
                    if (!next)
                    {
                        return {};
                    }
                    m_sub = next->get_next();
                    continue;
                }
                core::optional<In> next_sub = m_sub();
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

    struct impl
    {
        template <class T>
        auto operator()(const sequence<sequence<T>>& s) const -> sequence<T>
        {
            return sequence<T>{ next_function<T>{ s.get_next() } };
        }
    };

    auto operator()() const
    {
        return core::pipe(impl{});
    }
};

struct transform_join_fn
{
    template <class Func>
    struct impl
    {
        Func m_func;

        template <class T, class Out = sequence_underlying_type_t<std::invoke_result_t<Func, T>>>
        auto operator()(const sequence<T>& s) const -> sequence<Out>
        {
            return join_fn{}()(transform_fn{}(m_func)(s));
        }
    };

    template <class Func>
    auto operator()(Func&& func) const
    {
        return core::pipe(impl<std::decay_t<Func>>{ std::forward<Func>(func) });
    }
};

}  // namespace detail

static constexpr inline auto join = detail::join_fn{};
static constexpr inline auto transform_join = detail::transform_join_fn{};

}  // namespace seq
}  // namespace ferrugo
