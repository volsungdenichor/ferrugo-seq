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
    template <class Out, class Head>
    struct concatenate_impl
    {
        Head m_head;

        template <class T>
        auto operator()(T&& item) const -> Out
        {
            return concat(m_head, std::forward<T>(item));
        }
    };

    template <class Out, class Head>
    static auto concatenate(Head&& head) -> concatenate_impl<Out, std::decay_t<Head>>
    {
        return { std::forward<Head>(head) };
    }

    template <class Head, class T, class Out = concat_result_t<Head, T>>
    struct impl
    {
        sequence<T> m_seq;

        template <class Arg>
        auto operator()(Arg&& arg) const -> sequence<Out>
        {
            return m_seq |= transform(do_not_destructure(concatenate<Out>(Head{ std::forward<Arg>(arg) })));
        }
    };

    template <class T0, class T1, class Out = concat_result_t<T0, T1>>
    auto operator()(const sequence<T0>& seq0, const sequence<T1>& seq1) const -> sequence<Out>
    {
        return seq0 |= transform_join(do_not_destructure(impl<tuple<T0>, T1>{ seq1 }));
    }

    template <class T0, class T1, class T2, class Out = concat_result_t<T0, T1, T2>>
    auto operator()(const sequence<T0>& seq0, const sequence<T1>& seq1, const sequence<T2>& seq2) const -> sequence<Out>
    {
        return (*this)(seq0, seq1) |= transform_join(do_not_destructure(impl<tuple<T0, T1>, T2>{ seq2 }));
    }

    template <class T0, class T1, class T2, class T3, class Out = concat_result_t<T0, T1, T2, T3>>
    auto operator()(const sequence<T0>& seq0, const sequence<T1>& seq1, const sequence<T2>& seq2, const sequence<T3>& seq3)
        const -> sequence<Out>
    {
        return (*this)(seq0, seq1, seq2) |= transform_join(do_not_destructure(impl<tuple<T0, T1, T2>, T3>{ seq3 }));
    }
};

};  // namespace detail

static constexpr inline auto cartesian_product = detail::cartesian_product_fn{};

}  // namespace seq
}  // namespace ferrugo
