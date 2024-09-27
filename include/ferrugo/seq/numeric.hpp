#pragma once

#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{
namespace seq
{
namespace detail
{

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

}  // namespace detail

static constexpr inline auto iota = detail::iota_fn{};
static constexpr inline auto range = detail::range_fn{};

}  // namespace seq
}  // namespace ferrugo