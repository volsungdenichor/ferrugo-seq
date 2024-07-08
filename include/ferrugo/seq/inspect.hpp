#pragma once

#include <ferrugo/seq/sequence.hpp>
#include <ferrugo/seq/utils.hpp>

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

        auto operator()() const -> core::optional<In>
        {
            core::optional<In> res = m_next();
            if (res)
            {
                std::invoke(m_func, *res);
            }
            return res;
        }
    };

    template <class Func>
    struct impl
    {
        Func m_func;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<T>
        {
            return sequence<T>{ next_function<Func, T>{ m_func, s.get_next() } };
        }
    };

    template <class Func>
    auto operator()(Func&& func) const
    {
        return core::pipe(impl<std::decay_t<Func>>{ std::forward<Func>(func) });
    }
};

struct inspect_i_fn
{
    template <class Func>
    auto operator()(Func&& func) const
    {
        return inspect_fn{}(indexed_function(std::forward<Func>(func)));
    }
};

}  // namespace detail

static constexpr inline auto inspect = detail::inspect_fn{};
static constexpr inline auto inspect_i = detail::inspect_i_fn{};

}  // namespace seq
}  // namespace ferrugo
