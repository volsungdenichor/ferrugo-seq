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
                   [seq1](auto&& s0)
                   {
                       return seq1  //
                              |= transform([=](auto&& s1) { return concat(s0, s1); });
                   });
    }

    template <class T0, class T1, class T2, class Out = concat_result_t<T0, concat_result_t<T1, T2>>>
    auto operator()(const sequence<T0>& seq0, const sequence<T1>& seq1, const sequence<T2>& seq2) const -> sequence<Out>
    {
        return seq0  //
               |= transform_join(
                   [=](auto&& s0)
                   {
                       return seq1  //
                              |= transform_join(
                                  [=](auto&& s1)
                                  {
                                      return seq2  //
                                             |= seq::transform([=](auto&& s2) { return concat(s0, concat(s1, s2)); });
                                  });
                   });
    }

    template <
        class T0,
        class T1,
        class T2,
        class T3,
        class Out = concat_result_t<concat_result_t<T0, T1>, concat_result_t<T2, T3>>>
    auto operator()(const sequence<T0>& seq0, const sequence<T1>& seq1, const sequence<T2>& seq2, const sequence<T3>& seq3)
        const -> sequence<Out>
    {
        return seq0  //
               |= transform_join(
                   [=](auto&& s0)
                   {
                       return seq1  //
                              |= transform_join(
                                  [=](auto&& s1)
                                  {
                                      return seq2  //
                                             |= seq::transform_join(
                                                 [=](auto&& s2) {
                                                     return seq3
                                                            |= transform([=](auto&& s3)
                                                                         { return concat(concat(s0, s1), concat(s2, s3)); });
                                                 });
                                  });
                   });
    }
};

};  // namespace detail

static constexpr inline auto cartesian_product = detail::cartesian_product_fn{};

}  // namespace seq
}  // namespace ferrugo