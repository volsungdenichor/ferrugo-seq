#pragma once

#include <ferrugo/seq/invoke.hpp>
#include <ferrugo/seq/pipe.hpp>
#include <ferrugo/seq/sequence.hpp>

namespace ferrugo
{
namespace seq
{

struct slice_t
{
    std::ptrdiff_t start = 0;
    maybe<std::ptrdiff_t> stop = {};
    std::ptrdiff_t step = 1;
};

namespace detail
{

struct slice_fn
{
    template <class In>
    struct next_function
    {
        slice_t m_slice;
        next_function_t<In> m_next;
        mutable std::ptrdiff_t m_index = 0;

        auto operator()() const -> maybe<In>
        {
            while (true)
            {
                maybe<In> res = m_next();
                const std::ptrdiff_t index = m_index++;
                if (!res)
                {
                    break;
                }

                if (index < m_slice.start)
                {
                    continue;
                }

                if (m_slice.stop && (index >= *m_slice.stop))
                {
                    break;
                }

                if ((index - m_slice.start) % m_slice.step == 0)
                {
                    return res;
                }
            }
            return {};
        }
    };

    struct impl_t
    {
        slice_t m_slice;

        template <class T>
        auto operator()(const sequence<T>& s) const -> sequence<T>
        {
            return sequence<T>{ next_function<T>{ m_slice, s.get_next() } };
        }
    };

    auto operator()(slice_t slice) const -> pipe_t<impl_t>
    {
        return { { slice } };
    }
};

struct drop_fn
{
    auto operator()(std::ptrdiff_t n) const
    {
        return slice_fn{}({ n, {}, 1 });
    }
};

struct take_fn
{
    auto operator()(std::ptrdiff_t n) const
    {
        return slice_fn{}({ 0, n, 1 });
    }
};

struct step_fn
{
    auto operator()(std::ptrdiff_t n) const
    {
        return slice_fn{}({ 0, {}, n });
    }
};

}  // namespace detail

static constexpr inline auto slice = detail::slice_fn{};
static constexpr inline auto drop = detail::drop_fn{};
static constexpr inline auto take = detail::take_fn{};
static constexpr inline auto step = detail::step_fn{};

}  // namespace seq
}  // namespace ferrugo
