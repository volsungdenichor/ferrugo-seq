#pragma once

#include <ferrugo/seq/sequence.hpp>
#include <ferrugo/seq/transform.hpp>

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
    template <class T>
    constexpr auto operator()(T lower, T upper, std::ptrdiff_t count) const -> sequence<T>
    {
        return range_fn{}(count)
               |= seq::transform([=](std::ptrdiff_t n) -> T { return lower + n * (upper - lower) / (count - 1); });
    }
};

}  // namespace detail

static constexpr inline auto iota = detail::iota_fn{};
static constexpr inline auto range = detail::range_fn{};
static constexpr inline auto linspace = detail::linspace_fn{};

}  // namespace seq
}  // namespace ferrugo