#pragma once

#include <ferrugo/seq/range_traits.hpp>
#include <ferrugo/seq/sequence.hpp>
#include <memory>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct owning_fn
{
    template <class Range, class Iter, class Out>
    struct next_function
    {
        std::shared_ptr<Range> m_range;
        mutable Iter m_iter;

        next_function(std::shared_ptr<Range> range) : m_range(range), m_iter(std::begin(*m_range))
        {
        }

        auto operator()() const -> maybe<Out>
        {
            if (m_iter == std::end(*m_range))
            {
                return {};
            }
            return *m_iter++;
        }
    };

    template <class Range, class Out = range_reference_t<Range>>
    auto operator()(Range range) const -> sequence<Out>
    {
        return sequence<Out>{ next_function<Range, iterator_t<Range>, Out>{ std::make_shared<Range>(std::move(range)) } };
    }

    template <class T>
    auto operator()(const sequence<T>& s) const -> sequence<T>
    {
        return s;
    }
};

}  // namespace detail

static constexpr inline auto owning = detail::owning_fn{};

}  // namespace seq
}  // namespace ferrugo
