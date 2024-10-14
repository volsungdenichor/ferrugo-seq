#pragma once

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

struct single_fn
{
    template <class T>
    struct next_function
    {
        T m_value;
        mutable bool m_init = true;

        auto operator()() const -> maybe<const T&>
        {
            if (m_init)
            {
                m_init = false;
                return m_value;
            }
            return {};
        }
    };

    template <class T>
    auto operator()(T value) const -> sequence<const T&>
    {
        return sequence<const T&>{ next_function<T>{ std::move(value) } };
    }
};

struct repeat_fn
{
    template <class T>
    struct next_function
    {
        T m_value;

        auto operator()() const -> maybe<const T&>
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

struct iota_fn
{
    template <class In>
    struct next_function
    {
        mutable In m_current;
        auto operator()() const -> maybe<In>
        {
            return m_current++;
        }
    };

    template <class T>
    auto operator()(T init) const -> sequence<T>
    {
        return sequence<T>{ next_function<T>{ init } };
    }
};

struct range_fn
{
    template <class In>
    struct next_function
    {
        mutable In m_current;
        In m_upper;

        auto operator()() const -> maybe<In>
        {
            if (m_current >= m_upper)
            {
                return {};
            }
            return m_current++;
        }
    };

    template <class T>
    auto operator()(T lower, T upper) const -> sequence<T>
    {
        return sequence<T>{ next_function<T>{ lower, upper } };
    }

    template <class T>
    auto operator()(T upper) const -> sequence<T>
    {
        return (*this)(T{}, upper);
    }
};

struct init_fn
{
    template <class Func, class Out = invoke_result_t<Func, std::ptrdiff_t>>
    auto operator()(std::ptrdiff_t count, Func&& func) const -> sequence<Out>
    {
        return transform(std::forward<Func>(func))(range_fn{}(count));
    }
};

struct init_infinite_fn
{
    template <class Func, class Out = invoke_result_t<Func, std::ptrdiff_t>>
    auto operator()(Func&& func) const -> sequence<Out>
    {
        return transform(std::forward<Func>(func))(iota_fn{}(0));
    }
};

struct linspace_fn
{
    template <class T>
    auto operator()(T lower, T upper, std::ptrdiff_t count) const -> sequence<T>
    {
        return init_fn{}(count, [=](std::ptrdiff_t n) -> T { return lower + n * (upper - lower) / (count - 1); });
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
            auto&& [value, new_state] = *std::move(res);
            m_state = std::move(new_state);
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

template <class T>
static constexpr inline auto empty = detail::empty_fn<T>{};

static constexpr inline auto single = detail::single_fn{};
static constexpr inline auto repeat = detail::repeat_fn{};
static constexpr inline auto iota = detail::iota_fn{};
static constexpr inline auto range = detail::range_fn{};
static constexpr inline auto init = detail::init_fn{};
static constexpr inline auto init_infinite = detail::init_infinite_fn{};
static constexpr inline auto linspace = detail::linspace_fn{};
static constexpr inline auto unfold = detail::unfold_fn{};

}  // namespace seq
}  // namespace ferrugo
