#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct transform_fn
{
    template <class Func, class In, class Out>
    struct next_function
    {
        Func m_func;
        next_function_t<In> m_next;

        auto operator()() const -> core::optional<Out>
        {
            core::optional<In> res = m_next();
            if (!res)
            {
                return {};
            }
            return std::invoke(m_func, *res);
        }
    };

    template <class Func>
    struct impl
    {
        Func m_func;

        template <class T, class Out = std::invoke_result_t<Func, T>>
        auto operator()(const sequence<T>& s) const -> sequence<Out>
        {
            return sequence<Out>{ next_function<Func, T, Out>{ m_func, s.get_next() } };
        }
    };

    template <class Func>
    auto operator()(Func&& func) const
    {
        return core::pipe(impl<std::decay_t<Func>>{ std::forward<Func>(func) });
    }
};

}  // namespace detail

static constexpr inline auto transform = detail::transform_fn{};
}  // namespace seq
}  // namespace ferrugo
