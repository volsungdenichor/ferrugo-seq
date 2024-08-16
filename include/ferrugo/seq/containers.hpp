#pragma once

#include <ferrugo/seq/sequence.hpp>
#include <memory>

namespace ferrugo
{

namespace seq
{

namespace detail
{

struct view_fn
{
    template <class Range, class Iter, class Out>
    struct next_function
    {
        Range* m_range;
        mutable Iter m_iter;

        next_function(Range* range) : m_range(range), m_iter(std::begin(*m_range))
        {
        }

        auto operator()() const -> core::optional<Out>
        {
            if (m_iter == std::end(*m_range))
            {
                return {};
            }
            return *m_iter++;
        }
    };

    template <class Range, class Out = core::range_reference_t<Range>>
    auto operator()(Range& range) const -> sequence<Out>
    {
        return sequence<Out>{ next_function<Range, core::iterator_t<Range>, Out>{ &range } };
    }

    template <class T>
    auto operator()(const sequence<T>& s) const -> sequence<T>
    {
        return s;
    }
};

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

        auto operator()() const -> core::optional<Out>
        {
            if (m_iter == std::end(*m_range))
            {
                return {};
            }
            return *m_iter++;
        }
    };

    template <class Range, class Out = core::range_reference_t<Range>>
    auto operator()(Range range) const -> sequence<Out>
    {
        return sequence<Out>{ next_function<Range, core::iterator_t<Range>, Out>{
            std::make_shared<Range>(std::move(range)) } };
    }

    template <class T>
    auto operator()(const sequence<T>& s) const -> sequence<T>
    {
        return s;
    }
};

struct vec_fn
{
    template <class T, class... Tail>
    auto operator()(T head, Tail&&... tail) const -> sequence<const T&>
    {
        return owning_fn{}(std::vector<T>{ std::move(head), std::forward<Tail>(tail)... });
    }
};

}  // namespace detail

static constexpr inline auto view = detail::view_fn{};
static constexpr inline auto owning = detail::owning_fn{};
static constexpr inline auto vec = detail::vec_fn{};

}  // namespace seq
}  // namespace ferrugo
