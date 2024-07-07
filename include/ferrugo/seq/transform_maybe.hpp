#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct transform_maybe_fn
{
    template <class Func, class In, class Out>
    struct next_function
    {
        Func m_func;
        next_function_t<In> m_next;

        auto operator()() const -> core::optional<Out>
        {
            while (true)
            {
                core::optional<In> res = m_next();
                if (!res)
                {
                    break;
                }

                core::optional<Out> r = std::invoke(m_func, *res);
                if (r)
                {
                    return r;
                }
            }
            return {};
        }
    };

    template <class Func>
    struct impl
    {
        Func m_func;

        template <class T, class Out = core::optional_underlying_type_t<std::invoke_result_t<Func, T>>>
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

struct transform_maybe_i_fn
{
    template <class Func, class In, class Out>
    struct next_function
    {
        Func m_func;
        next_function_t<In> m_next;
        mutable std::ptrdiff_t m_index = 0;

        auto operator()() const -> core::optional<Out>
        {
            while (true)
            {
                core::optional<In> res = m_next();
                if (!res)
                {
                    break;
                }

                core::optional<Out> r = std::invoke(m_func, m_index++, *res);
                if (r)
                {
                    return r;
                }
            }
            return {};
        }
    };

    template <class Func>
    struct impl
    {
        Func m_func;

        template <class T, class Out = core::optional_underlying_type_t<std::invoke_result_t<Func, std::ptrdiff_t, T>>>
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

static constexpr inline auto transform_maybe = detail::transform_maybe_fn{};
static constexpr inline auto transform_maybe_i = detail::transform_maybe_i_fn{};
}  // namespace seq
}  // namespace ferrugo
