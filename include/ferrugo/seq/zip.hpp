#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

template <class... Types>
struct to_tuple
{
    template <class... Args>
    auto operator()(Args&&... args) const -> std::tuple<Types...>
    {
        return std::forward_as_tuple(std::forward<Args>(args)...);
    }
};

struct zip_transform_fn
{
    template <class Func, class Out, class In0, class In1, class In2 = void, class In3 = void>
    struct next_function;

    template <class Func, class Out, class In0, class In1, class In2, class In3>
    struct next_function
    {
        Func m_func;
        next_function_t<In0> m_next0;
        next_function_t<In1> m_next1;
        next_function_t<In2> m_next2;
        next_function_t<In3> m_next3;

        auto operator()() const -> core::optional<Out>
        {
            core::optional<In0> n0 = m_next0();
            core::optional<In1> n1 = m_next1();
            core::optional<In2> n2 = m_next2();
            core::optional<In3> n3 = m_next3();
            if (n0 && n1 && n2 && n3)
            {
                return std::invoke(m_func, *n0, *n1, *n2, *n3);
            }
            return {};
        }
    };

    template <class Func, class Out, class In0, class In1, class In2>
    struct next_function<Func, Out, In0, In1, In2, void>
    {
        Func m_func;
        next_function_t<In0> m_next0;
        next_function_t<In1> m_next1;
        next_function_t<In2> m_next2;

        auto operator()() const -> core::optional<Out>
        {
            core::optional<In0> n0 = m_next0();
            core::optional<In1> n1 = m_next1();
            core::optional<In2> n2 = m_next2();
            if (n0 && n1 && n2)
            {
                return std::invoke(m_func, *n0, *n1, *n2);
            }
            return {};
        }
    };

    template <class Func, class Out, class In0, class In1>
    struct next_function<Func, Out, In0, In1, void, void>
    {
        Func m_func;
        next_function_t<In0> m_next0;
        next_function_t<In1> m_next1;

        auto operator()() const -> core::optional<Out>
        {
            core::optional<In0> n0 = m_next0();
            core::optional<In1> n1 = m_next1();
            if (n0 && n1)
            {
                return std::invoke(m_func, *n0, *n1);
            }
            return {};
        }
    };

    template <class Func, class T0, class T1, class T2, class T3, class Out = std::invoke_result_t<Func, T0, T1, T2, T3>>
    auto operator()(
        Func func, const sequence<T0>& s0, const sequence<T1>& s1, const sequence<T2>& s2, const sequence<T3>& s3) const
        -> sequence<Out>
    {
        return sequence<Out>{ next_function<Func, Out, T0, T1, T2, T3>{
            func, s0.get_next(), s1.get_next(), s2.get_next(), s3.get_next() } };
    }

    template <class Func, class T0, class T1, class T2, class Out = std::invoke_result_t<Func, T0, T1, T2>>
    auto operator()(Func func, const sequence<T0>& s0, const sequence<T1>& s1, const sequence<T2>& s2) const -> sequence<Out>
    {
        return sequence<Out>{ next_function<Func, Out, T0, T1, T2>{ func, s0.get_next(), s1.get_next(), s2.get_next() } };
    }

    template <class Func, class T0, class T1, class Out = std::invoke_result_t<Func, T0, T1>>
    auto operator()(Func func, const sequence<T0>& s0, const sequence<T1>& s1) const -> sequence<Out>
    {
        return sequence<Out>{ next_function<Func, Out, T0, T1>{ func, s0.get_next(), s1.get_next() } };
    }
};

struct zip_fn
{
    template <class T0, class T1, class T2, class T3, class Out = std::tuple<T0, T1, T2, T3>>
    auto operator()(const sequence<T0>& s0, const sequence<T1>& s1, const sequence<T2>& s2, const sequence<T3>& s3) const
        -> sequence<Out>
    {
        return zip_transform_fn{}(to_tuple<T0, T1, T2, T3>{}, s0, s1, s2, s3);
    }

    template <class T0, class T1, class T2, class Out = std::tuple<T0, T1, T2>>
    auto operator()(const sequence<T0>& s0, const sequence<T1>& s1, const sequence<T2>& s2) const -> sequence<Out>
    {
        return zip_transform_fn{}(to_tuple<T0, T1, T2>{}, s0, s1, s2);
    }

    template <class T0, class T1, class Out = std::tuple<T0, T1>>
    auto operator()(const sequence<T0>& s0, const sequence<T1>& s1) const -> sequence<Out>
    {
        return zip_transform_fn{}(to_tuple<T0, T1>{}, s0, s1);
    }
};

}  // namespace detail

static constexpr inline auto zip_transform = detail::zip_transform_fn{};
static constexpr inline auto zip = detail::zip_fn{};

}  // namespace seq
}  // namespace ferrugo