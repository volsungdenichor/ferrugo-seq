#pragma once

#include <algorithm>
#include <ferrugo/seq/sequence.hpp>
#include <ferrugo/seq/slice.hpp>
#include <ferrugo/seq/utils.hpp>

namespace ferrugo
{

namespace seq

{

namespace detail
{

struct maybe_front_fn
{
    template <class T>
    auto operator()(const sequence<T>& s) const -> core::optional<T>
    {
        return s.get_next()();
    }
};

static const inline auto maybe_front = core::pipe(detail::maybe_front_fn{});

struct nth_fn
{
    auto operator()(std::ptrdiff_t index) const
    {
        return seq::drop(index) |= maybe_front;
    }
};

struct find_if_fn
{
    template <class Pred>
    auto operator()(Pred&& pred) const
    {
        return seq::drop_while(std::not_fn(std::forward<Pred>(pred))) |= maybe_front;
    }
};

struct all_of_fn
{
    template <class Pred>
    struct impl
    {
        Pred m_pred;

        template <class T>
        auto operator()(const sequence<T>& s) const
        {
            return std::all_of(std::begin(s), std::end(s), std::ref(m_pred));
        }
    };

    template <class Pred>
    auto operator()(Pred&& pred) const
    {
        return core::pipe(impl<std::decay_t<Pred>>{ std::forward<Pred>(pred) });
    }
};

struct any_of_fn
{
    template <class Pred>
    struct impl
    {
        Pred m_pred;

        template <class T>
        auto operator()(const sequence<T>& s) const
        {
            return std::any_of(std::begin(s), std::end(s), std::ref(m_pred));
        }
    };

    template <class Pred>
    auto operator()(Pred&& pred) const
    {
        return core::pipe(impl<std::decay_t<Pred>>{ std::forward<Pred>(pred) });
    }
};

struct for_each_fn
{
    template <class Func>
    struct impl
    {
        Func m_func;

        template <class T>
        void operator()(const sequence<T>& s) const
        {
            std::for_each(std::begin(s), std::end(s), std::ref(m_func));
        }
    };

    template <class Func>
    auto operator()(Func&& func) const
    {
        return core::pipe(impl<std::decay_t<Func>>{ std::forward<Func>(func) });
    }
};

struct for_each_i_fn
{
    template <class Func>
    auto operator()(Func&& func) const
    {
        return for_each_fn{}(indexed_function(std::forward<Func>(func)));
    }
};

struct fold_fn
{
    template <class V, class Func>
    struct impl
    {
        V m_init;
        Func m_func;

        template <class T>
        auto operator()(const sequence<T>& s) const -> V
        {
            V result = m_init;
            auto b = std::begin(s);
            const auto e = std::end(s);
            for (; b != e; ++b)
            {
                result = m_func(result, *b);
            }
            return result;
        }
    };

    template <class T, class Func>
    auto operator()(T init, Func&& func) const
    {
        return core::pipe(impl<T, std::decay_t<Func>>{ std::move(init), std::forward<Func>(func) });
    }
};

}  // namespace detail

static constexpr inline auto maybe_front = detail::maybe_front;
static constexpr inline auto nth = detail::nth_fn{};
static constexpr inline auto find_if = detail::find_if_fn{};
static constexpr inline auto all_of = detail::all_of_fn{};
static constexpr inline auto any_of = detail::any_of_fn{};
static constexpr inline auto for_each = detail::for_each_fn{};
static constexpr inline auto for_each_i = detail::for_each_i_fn{};
static constexpr inline auto fold = detail::fold_fn{};
}  // namespace seq
}  // namespace ferrugo
