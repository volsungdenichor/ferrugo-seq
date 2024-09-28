#pragma once

#include <ferrugo/seq/transform_join.hpp>

namespace ferrugo
{
namespace seq
{

namespace detail
{

struct cartesian_product_fn
{
    template <class Outer, class Inner>
    struct transform_inner
    {
        Outer m_outer;

        template <class I>
        auto operator()(I&& inner) const -> concat_result_t<Outer, Inner>
        {
            return concat(m_outer, std::forward<I>(inner));
        }
    };

    template <class Outer, class Inner>
    struct transform_outer
    {
        sequence<Inner> m_inner;

        template <class O>
        auto operator()(O&& outer) const -> sequence<concat_result_t<Outer, Inner>>
        {
            return transform(transform_inner<Outer, Inner>{ std::forward<O>(outer) })(m_inner);
        }
    };

    template <class Outer, class Inner, class Out = concat_result_t<Outer, Inner>>
    auto operator()(const sequence<Outer>& outer, const sequence<Inner>& inner) const -> sequence<Out>
    {
        return transform_join(transform_outer<Outer, Inner>{ inner })(outer);
    }
};

};  // namespace detail

static constexpr inline auto cartesian_product = detail::cartesian_product_fn{};

}  // namespace seq
}  // namespace ferrugo