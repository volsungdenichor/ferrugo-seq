#pragma once

#include <ferrugo/seq/range_traits.hpp>
#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{
namespace seq
{

namespace detail
{

struct from_fn
{
    template <class Iter, class Out>
    struct next_function
    {
        mutable Iter m_iter;
        Iter m_end;

        next_function(Iter begin, Iter end) : m_iter(begin), m_end(end)
        {
        }

        auto operator()() const -> maybe<Out>
        {
            if (m_iter == m_end)
            {
                return {};
            }
            return *m_iter++;
        }
    };

    template <class Iter, class Out = iter_reference_t<Iter>>
    auto operator()(Iter begin, Iter end) const -> sequence<Out>
    {
        return sequence<Out>{ next_function<Iter, Out>{ begin, end } };
    }
};

}  // namespace detail

static constexpr inline auto from = detail::from_fn{};

}  // namespace seq
}  // namespace ferrugo
