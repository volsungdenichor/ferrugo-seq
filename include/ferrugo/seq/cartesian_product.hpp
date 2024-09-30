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
    struct map_fn
    {
        template <class Func, class In, class Out>
        struct next_function
        {
            Func m_func;
            next_function_t<In> m_next;

            auto operator()() const -> maybe<Out>
            {
                maybe<In> res = m_next();
                if (!res)
                {
                    return {};
                }
                return std::invoke(m_func, *res);
            }
        };

        template <class Func, class T, class Out = std::invoke_result_t<Func, T>>
        auto operator()(Func&& func, const sequence<T>& s) const -> sequence<Out>
        {
            return sequence<Out>{ next_function<std::decay_t<Func>, T, Out>{ std::forward<Func>(func), s.get_next() } };
        }
    };

    static constexpr auto map = map_fn{};

    struct flat_map_fn
    {
        template <class Func, class T>
        auto operator()(Func&& func, const sequence<T>& s) const
        {
            return join(map(std::forward<Func>(func), s));
        }
    };

    static constexpr auto flat_map = flat_map_fn{};

    template <class Out, class Head>
    struct concatenate
    {
        Head m_head;

        template <class T>
        auto operator()(T&& item) const -> Out
        {
            return concat(m_head, std::forward<T>(item));
        }
    };

    template <class Out, class Head>
    static auto make_concatenate(Head head) -> concatenate<Out, Head>
    {
        return concatenate<Out, Head>{ std::move(head) };
    }

    template <class Head, class T, class Out = concat_result_t<Head, T>>
    struct impl
    {
        sequence<T> m_seq;

        template <class Arg>
        auto operator()(Arg&& arg) const
        {
            return map(make_concatenate<Out>(Head{ std::forward<Arg>(arg) }), m_seq);
        }
    };

    template <class T0, class T1, class Out = concat_result_t<T0, T1>>
    auto operator()(const sequence<T0>& seq0, const sequence<T1>& seq1) const -> sequence<Out>
    {
        return flat_map(impl<tuple<T0>, T1>{ seq1 }, seq0);
    }

    template <class T0, class T1, class T2, class Out = concat_result_t<T0, T1, T2>>
    auto operator()(const sequence<T0>& seq0, const sequence<T1>& seq1, const sequence<T2>& seq2) const -> sequence<Out>
    {
        return flat_map(impl<tuple<T0, T1>, T2>{ seq2 }, (*this)(seq0, seq1));
    }

    template <class T0, class T1, class T2, class T3, class Out = concat_result_t<T0, T1, T2, T3>>
    auto operator()(const sequence<T0>& seq0, const sequence<T1>& seq1, const sequence<T2>& seq2, const sequence<T3>& seq3)
        const -> sequence<Out>
    {
        return flat_map(impl<tuple<T0, T1, T2>, T3>{ seq3 }, (*this)(seq0, seq1, seq2));
    }
};

};  // namespace detail

static constexpr inline auto cartesian_product = detail::cartesian_product_fn{};

}  // namespace seq
}  // namespace ferrugo