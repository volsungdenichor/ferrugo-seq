#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct chain_fn
{
    template <class In>
    struct next_function
    {
        next_function_t<In> m_first;
        next_function_t<In> m_second;
        mutable bool m_first_finished = false;

        auto operator()() const -> maybe<In>
        {
            if (!m_first_finished)
            {
                maybe<In> n = m_first();
                if (n)
                {
                    return n;
                }
                else
                {
                    m_first_finished = true;
                }
            }
            return m_second();
        }
    };

    template <class T0, class T1, class T2, class T3, class Out = std::common_type_t<T0, T1, T2, T3>>
    auto operator()(const sequence<T0>& s0, const sequence<T1>& s1, const sequence<T2>& s2, const sequence<T3>& s3) const
        -> sequence<Out>
    {
        return (*this)((*this)((*this)(s0, s1), s2), s3);
    }

    template <class T0, class T1, class T2, class Out = std::common_type_t<T0, T1, T2>>
    auto operator()(const sequence<T0>& s0, const sequence<T1>& s1, const sequence<T2>& s2) const -> sequence<Out>
    {
        return (*this)((*this)(s0, s1), s2);
    }

    template <class T0, class T1, class Out = std::common_type_t<T0, T1>>
    auto operator()(const sequence<T0>& s0, const sequence<T1>& s1) const -> sequence<Out>
    {
        return (*this)(sequence<Out>{ s0 }, sequence<Out>{ s1 });
    }

    template <class T>
    auto operator()(const sequence<T>& lhs, const sequence<T>& rhs) const -> sequence<T>
    {
        return sequence<T>{ next_function<T>{ lhs.get_next(), rhs.get_next() } };
    }
};

}  // namespace detail

static constexpr inline auto chain = detail::chain_fn{};

}  // namespace seq
}  // namespace ferrugo
