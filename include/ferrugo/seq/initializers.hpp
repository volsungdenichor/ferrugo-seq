#pragma once

#include <ferrugo/seq/numeric.hpp>
#include <ferrugo/seq/transform.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct init_fn
{
    template <class Func, class Out = invoke_result_t<Func, std::ptrdiff_t>>
    auto operator()(std::ptrdiff_t count, Func&& func) const -> sequence<Out>
    {
        return transform(std::forward<Func>(func))(range(count));
    }
};

struct init_infinite_fn
{
    template <class Func, class Out = invoke_result_t<Func, std::ptrdiff_t>>
    auto operator()(Func&& func) const -> sequence<Out>
    {
        return transform(std::forward<Func>(func))(iota(0));
    }
};

struct unfold_fn
{
    template <class Func, class S, class Out>
    struct next_function
    {
        Func m_func;
        mutable S m_state;

        auto operator()() const -> maybe<Out>
        {
            auto res = m_func(m_state);
            if (!res)
            {
                return {};
            }
            auto [value, state] = *std::move(res);
            m_state = std::move(state);
            return value;
        }
    };

    template <
        class S,
        class Func,
        class OptRes = invoke_result_t<Func, const S&>,
        class Res = maybe_underlying_type_t<OptRes>,
        class Out = std::tuple_element_t<0, Res>>
    auto operator()(S state, Func&& func) const -> sequence<Out>
    {
        return sequence<Out>{ next_function<std::decay_t<Func>, S, Out>{ std::forward<Func>(func), std::move(state) } };
    }
};

}  // namespace detail

static constexpr inline auto init = detail::init_fn{};
static constexpr inline auto init_infinite = detail::init_infinite_fn{};
static constexpr inline auto unfold = detail::unfold_fn{};

}  // namespace seq
}  // namespace ferrugo
