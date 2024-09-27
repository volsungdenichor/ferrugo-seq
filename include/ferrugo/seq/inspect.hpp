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

struct inspect_fn
{
    template <class Func, class In>
    struct next_function
    {
        Func m_func;
        next_function_t<In> m_next;

        auto operator()() const -> maybe<In>
        {
            maybe<In> res = m_next();
            if (res)
            {
                invoke(m_func, *res);
            }
            return res;
        }
    };

    template <class Func>
    struct impl_t
    {
        Func m_func;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<T>
        {
            return sequence<T>{ next_function<Func, T>{ m_func, s.get_next() } };
        }
    };

    template <class Func>
    auto operator()(Func&& func) const -> pipe_t<impl_t<std::decay_t<Func>>>
    {
        return { { std::forward<Func>(func) } };
    }
};

struct inspect_i_fn
{
    template <class Func, class In>
    struct next_function
    {
        Func m_func;
        next_function_t<In> m_next;
        mutable std::ptrdiff_t m_index = 0;

        auto operator()() const -> maybe<In>
        {
            maybe<In> res = m_next();
            if (res)
            {
                invoke(m_func, concat(m_index++, *res));
            }
            return res;
        }
    };

    template <class Func>
    struct impl_t
    {
        Func m_func;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<T>
        {
            return sequence<T>{ next_function<Func, T>{ m_func, s.get_next() } };
        }
    };

    template <class Func>
    auto operator()(Func&& func) const -> pipe_t<impl_t<std::decay_t<Func>>>
    {
        return { { std::forward<Func>(func) } };
    }
};

}  // namespace detail

static constexpr inline auto inspect = detail::inspect_fn{};
static constexpr inline auto inspect_i = detail::inspect_i_fn{};

}  // namespace seq
}  // namespace ferrugo
