#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{
namespace seq
{

namespace detail
{

struct zip_fn
{
    template <class In0, class In1 = void, class In2 = void, class In3 = void>
    struct next_function;

    template <class In0, class In1, class In2, class In3>
    struct next_function
    {
        next_function_t<In0> m_next0;
        next_function_t<In1> m_next1;
        next_function_t<In2> m_next2;
        next_function_t<In3> m_next3;

        auto operator()() const -> maybe<tuple<In0, In1, In2, In3>>
        {
            maybe<In0> n0 = m_next0();
            maybe<In1> n1 = m_next1();
            maybe<In2> n2 = m_next2();
            maybe<In3> n3 = m_next3();
            if (n0 && n1 && n2 && n3)
            {
                return tuple<In0, In1, In2, In3>{ *n0, *n1, *n2, *n3 };
            }
            return {};
        }
    };

    template <class In0, class In1, class In2>
    struct next_function<In0, In1, In2, void>
    {
        next_function_t<In0> m_next0;
        next_function_t<In1> m_next1;
        next_function_t<In2> m_next2;

        auto operator()() const -> maybe<tuple<In0, In1, In2>>
        {
            maybe<In0> n0 = m_next0();
            maybe<In1> n1 = m_next1();
            maybe<In2> n2 = m_next2();
            if (n0 && n1 && n2)
            {
                return tuple<In0, In1, In2>{ *n0, *n1, *n2 };
            }
            return {};
        }
    };

    template <class In0, class In1>
    struct next_function<In0, In1, void, void>
    {
        next_function_t<In0> m_next0;
        next_function_t<In1> m_next1;

        auto operator()() const -> maybe<tuple<In0, In1>>
        {
            maybe<In0> n0 = m_next0();
            maybe<In1> n1 = m_next1();
            if (n0 && n1)
            {
                return tuple<In0, In1>{ *n0, *n1 };
            }
            return {};
        }
    };

    template <class In0>
    struct next_function<In0, void, void, void>
    {
        next_function_t<In0> m_next0;

        auto operator()() const -> maybe<tuple<In0>>
        {
            maybe<In0> n0 = m_next0();
            if (n0)
            {
                return tuple<In0>{ *n0 };
            }
            return {};
        }
    };

    template <class T0, class T1, class T2, class T3, class Out = tuple<T0, T1, T2, T3>>
    auto operator()(const sequence<T0>& s0, const sequence<T1>& s1, const sequence<T2>& s2, const sequence<T3>& s3) const
        -> sequence<Out>
    {
        return sequence<Out>{ next_function<T0, T1, T2, T3>{ s0.get_next(), s1.get_next(), s2.get_next(), s3.get_next() } };
    }

    template <class T0, class T1, class T2, class Out = tuple<T0, T1, T2>>
    auto operator()(const sequence<T0>& s0, const sequence<T1>& s1, const sequence<T2>& s2) const -> sequence<Out>
    {
        return sequence<Out>{ next_function<T0, T1, T2>{ s0.get_next(), s1.get_next(), s2.get_next() } };
    }

    template <class T0, class T1, class Out = tuple<T0, T1>>
    auto operator()(const sequence<T0>& s0, const sequence<T1>& s1) const -> sequence<Out>
    {
        return sequence<Out>{ next_function<T0, T1>{ s0.get_next(), s1.get_next() } };
    }

    template <class T0, class Out = tuple<T0>>
    auto operator()(const sequence<T0>& s0) const -> sequence<Out>
    {
        return sequence<Out>{ next_function<T0>{ s0.get_next() } };
    }
};

}  // namespace detail

static constexpr inline auto zip = detail::zip_fn{};

}  // namespace seq
}  // namespace ferrugo
