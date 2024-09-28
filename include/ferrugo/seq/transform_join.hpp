#pragma once

#include <ferrugo/seq/join.hpp>
#include <ferrugo/seq/transform.hpp>

namespace ferrugo
{
namespace seq
{
namespace detail
{

struct transform_join_fn
{
    template <class Func>
    struct impl_t
    {
        Func m_func;

        template <class T>
        auto operator()(const sequence<T>& s) const
        {
            return join(transform(m_func)(s));
        }
    };

    template <class Func>
    auto operator()(Func&& func) const -> pipe_t<impl_t<std::decay_t<Func>>>
    {
        return { { std::forward<Func>(func) } };
    }
};

}  // namespace detail

static constexpr inline auto transform_join = detail::transform_join_fn{};

}  // namespace seq
}  // namespace ferrugo