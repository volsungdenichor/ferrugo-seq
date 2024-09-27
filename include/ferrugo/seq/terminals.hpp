#pragma once

#include <algorithm>
#include <ferrugo/seq/pipe.hpp>
#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{

namespace seq

{

namespace detail
{

struct maybe_front_fn
{
    template <class T>
    auto operator()(const sequence<T>& s) const -> maybe<T>
    {
        return s.get_next()();
    }
};

static const inline auto maybe_front = pipe_t<detail::maybe_front_fn>{ detail::maybe_front_fn{} };

struct nth_fn
{
    struct impl_t
    {
        std::ptrdiff_t m_index;

        template <class T>
        auto operator()(const sequence<T>& s) const -> maybe<T>
        {
            return maybe_front(drop(m_index)(s));
        }
    };

    auto operator()(std::ptrdiff_t index) const -> pipe_t<impl_t>
    {
        return { { index } };
    }
};

struct find_if_fn
{
    template <class Pred>
    struct impl_t
    {
        Pred m_pred;

        template <class T>
        auto operator()(const sequence<T>& s) const -> maybe<T>
        {
            return maybe_front(drop_while(std::not_fn(m_pred))(s));
        }
    };

    template <class Pred>
    auto operator()(Pred&& pred) const -> pipe_t<impl_t<std::decay_t<Pred>>>
    {
        return { { std::forward<Pred>(pred) } };
    }
};

struct index_of_fn
{
    template <class Pred>
    struct impl_t
    {
        Pred m_pred;

        template <class T>
        auto operator()(const sequence<T>& s) const -> maybe<std::ptrdiff_t>
        {
            std::ptrdiff_t index = 0;
            auto it = std::begin(s);
            const auto end = std::end(s);
            for (; it != end; ++it, ++index)
            {
                if (std::invoke(m_pred, *it))
                {
                    return index;
                }
            }
            return {};
        }
    };

    template <class Pred>
    auto operator()(Pred&& pred) const -> pipe_t<impl_t<std::decay_t<Pred>>>
    {
        return { { std::forward<Pred>(pred) } };
    }
};

struct for_each_fn
{
    template <class Func>
    struct impl_t
    {
        Func m_func;

        template <class T>
        void operator()(const sequence<T>& s) const
        {
            auto it = std::begin(s);
            const auto end = std::end(s);
            for (; it != end; ++it)
            {
                invoke(m_func, *it);
            }
        }
    };

    template <class Func>
    auto operator()(Func&& func) const -> pipe_t<impl_t<std::decay_t<Func>>>
    {
        return { { std::forward<Func>(func) } };
    }
};

struct for_each_i_fn
{
    template <class Func>
    struct impl_t
    {
        Func m_func;

        template <class T>
        void operator()(const sequence<T>& s) const
        {
            auto it = std::begin(s);
            const auto end = std::end(s);
            std::ptrdiff_t index = 0;
            for (; it != end; ++it, ++index)
            {
                invoke(m_func, concat(index, *it));
            }
        }
    };

    template <class Func>
    auto operator()(Func&& func) const -> pipe_t<impl_t<std::decay_t<Func>>>
    {
        return { { std::forward<Func>(func) } };
    }
};

struct fold_fn
{
    template <class V, class Func>
    struct impl_t
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
    auto operator()(T init, Func&& func) const -> pipe_t<impl_t<T, std::decay_t<Func>>>
    {
        return { { std::move(init), std::forward<Func>(func) } };
    }
};

struct copy_fn
{
    template <class Out>
    struct impl_t
    {
        Out m_out;

        template <class T>
        auto operator()(const sequence<T>& s) const -> Out
        {
            return std::copy(std::begin(s), std::end(s), m_out);
        }
    };

    template <class Out>
    auto operator()(Out out) const -> pipe_t<impl_t<Out>>
    {
        return { { std::move(out) } };
    }
};

}  // namespace detail

static constexpr inline auto maybe_front = detail::maybe_front;
static constexpr inline auto nth = detail::nth_fn{};
static constexpr inline auto find_if = detail::find_if_fn{};
static constexpr inline auto index_of = detail::index_of_fn{};
static constexpr inline auto for_each = detail::for_each_fn{};
static constexpr inline auto for_each_i = detail::for_each_i_fn{};
static constexpr inline auto fold = detail::fold_fn{};
static constexpr inline auto copy = detail::copy_fn{};

}  // namespace seq
}  // namespace ferrugo
