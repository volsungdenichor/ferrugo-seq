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
        auto operator()() const -> core::optional<In>
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

        auto operator()() const -> core::optional<In>
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

struct linspace_fn
{
    template <class In>
    struct next_function
    {
        In m_lower;
        In m_upper;
        ptrdiff_t m_count;
        mutable std::ptrdiff_t m_current = 0;

        auto operator()() const -> core::optional<In>
        {
            if (m_current >= m_upper)
            {
                return {};
            }
            std::ptrdiff_t n = m_current++;
            return m_lower + n * (m_upper - m_lower) / (m_count - 1);
        };
    };

    template <class T>
    constexpr auto operator()(T lower, T upper, std::ptrdiff_t count) const -> sequence<T>
    {
        static_assert(std::is_floating_point_v<T>, "linspace: floating point type expected");
        return sequence<T>{ next_function<T>{ lower, upper, count } };
    }
};

}  // namespace detail

static constexpr inline auto iota = detail::iota_fn{};
static constexpr inline auto range = detail::range_fn{};
static constexpr inline auto linspace = detail::linspace_fn{};

}  // namespace seq
}  // namespace ferrugo