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
    template <class T0, class T1, class Out = concat_result_t<T0, T1>>
    auto operator()(const sequence<T0>& seq0, const sequence<T1>& seq1) const -> sequence<Out>
    {
        return seq0  //
               |= transform_join(
                   [seq1](auto&& s0) -> sequence<Out>
                   {
                       return seq1  //
                              |= transform([=](auto&& s1) -> Out { return concat(s0, s1); });
                   });
    }

    template <class T0, class T1, class T2, class Out = concat_result_t<T0, T1, T2>>
    auto operator()(const sequence<T0>& seq0, const sequence<T1>& seq1, const sequence<T2>& seq2) const -> sequence<Out>
    {
        return (*this)(seq0, seq1)  //
               |= transform_join(
                   [seq2](auto&& s0, auto&& s1) -> sequence<Out>
                   {
                       return seq2  //
                              |= transform([=](auto&& s2) -> Out { return concat(s0, s1, s2); });
                   });
    }

    template <class T0, class T1, class T2, class T3, class Out = concat_result_t<T0, T1, T2, T3>>
    auto operator()(const sequence<T0>& seq0, const sequence<T1>& seq1, const sequence<T2>& seq2, const sequence<T3>& seq3)
        const -> sequence<Out>
    {
        return (*this)(seq0, seq1, seq2)  //
               |= transform_join(
                   [seq3](auto&& s0, auto&& s1, auto&& s2) -> sequence<Out>
                   {
                       return seq3  //
                              |= transform([=](auto&& s3) -> Out { return concat(s0, s1, s2, s3); });
                   });
    }
};

};  // namespace detail

static constexpr inline auto cartesian_product = detail::cartesian_product_fn{};

}  // namespace seq
}  // namespace ferrugo