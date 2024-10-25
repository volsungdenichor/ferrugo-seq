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

struct transform_maybe_fn
{
    template <class Func, class In, class Out>
    struct next_function
    {
        Func m_func;
        next_function_t<In> m_next;

        auto operator()() const -> maybe<Out>
        {
            while (true)
            {
                maybe<In> res = m_next();
                if (!res)
                {
                    break;
                }

                maybe<Out> r = invoke(m_func, *std::move(res));
                if (r)
                {
                    return r;
                }
            }
            return {};
        }
    };

    template <class Func>
    struct impl_t
    {
        Func m_func;

        template <class T, class Out = maybe_underlying_type_t<invoke_result_t<Func, T>>>
        auto operator()(const sequence<T>& s) const -> sequence<Out>
        {
            return sequence<Out>{ next_function<Func, T, Out>{ m_func, s.get_next() } };
        }
    };

    template <class Func>
    auto operator()(Func&& func) const -> pipe_t<impl_t<std::decay_t<Func>>>
    {
        return { { std::forward<Func>(func) } };
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

        auto operator()() const -> maybe<Out>
        {
            while (true)
            {
                maybe<In> res = m_next();
                if (!res)
                {
                    break;
                }

                maybe<Out> r = invoke(m_func, tuplify(m_index++, *std::move(res)));
                if (r)
                {
                    return r;
                }
            }
            return {};
        }
    };

    template <class Func>
    struct impl_t
    {
        Func m_func;

        template <class T, class Out = maybe_underlying_type_t<invoke_result_t<Func, tuplify_result_t<std::ptrdiff_t, T>>>>
        auto operator()(const sequence<T>& s) const -> sequence<Out>
        {
            return sequence<Out>{ next_function<Func, T, Out>{ m_func, s.get_next() } };
        }
    };

    template <class Func>
    auto operator()(Func&& func) const -> pipe_t<impl_t<std::decay_t<Func>>>
    {
        return { { std::forward<Func>(func) } };
    }
};

}  // namespace detail

static constexpr inline auto transform_maybe = detail::transform_maybe_fn{};
static constexpr inline auto transform_maybe_i = detail::transform_maybe_i_fn{};

}  // namespace seq
}  // namespace ferrugo
