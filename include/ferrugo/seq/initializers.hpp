#pragma once

#include <ferrugo/seq/numeric.hpp>
#include <ferrugo/seq/sequence.hpp>
#include <ferrugo/seq/transform.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

template <class T>
struct empty_fn
{
    auto operator()() const -> sequence<T>
    {
        return {};
    }
};

struct repeat_fn
{
    template <class T>
    struct next_function
    {
        T m_value;

        auto operator()() const -> core::optional<const T&>
        {
            return m_value;
        }
    };

    template <class T>
    auto operator()(T value) const -> sequence<const T&>
    {
        return sequence<const T&>{ next_function<T>{ std::move(value) } };
    }
};

struct init_fn
{
    template <class Func, class Out = std::invoke_result_t<Func, std::ptrdiff_t>>
    auto operator()(std::ptrdiff_t count, Func&& func) const -> sequence<Out>
    {
        return range(count) |= transform(std::forward<Func>(func));
    }
};

struct init_infinite_fn
{
    template <class Func, class Out = std::invoke_result_t<Func, std::ptrdiff_t>>
    auto operator()(Func&& func) const -> sequence<Out>
    {
        return iota(0) |= transform(std::forward<Func>(func));
    }
};

struct unfold_fn
{
    template <class Func, class S, class Out>
    struct next_function
    {
        Func m_func;
        mutable S m_state;

        auto operator()() const -> core::optional<Out>
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
        class OptRes = std::invoke_result_t<Func, const S&>,
        class Res = core::optional_underlying_type_t<OptRes>,
        class Out = std::tuple_element_t<0, Res>>
    auto operator()(S state, Func&& func) const -> sequence<Out>
    {
        return sequence<Out>{ next_function<std::decay_t<Func>, S, Out>{ std::forward<Func>(func), std::move(state) } };
    }
};

}  // namespace detail

template <class T>
static constexpr inline auto empty = detail::empty_fn<T>{};

static constexpr inline auto repeat = detail::repeat_fn{};
static constexpr inline auto init = detail::init_fn{};
static constexpr inline auto init_infinite = detail::init_infinite_fn{};
static constexpr inline auto unfold = detail::unfold_fn{};
}  // namespace seq
}  // namespace ferrugo
