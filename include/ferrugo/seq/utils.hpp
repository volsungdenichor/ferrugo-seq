#pragma once

#include <functional>

namespace ferrugo
{
namespace seq
{
namespace detail
{
struct indexed_function_fn
{
    template <class Func>
    struct impl
    {
        Func m_func;
        mutable std::ptrdiff_t m_index = 0;

        template <class... Args>
        auto operator()(Args&&... args) const -> decltype(std::invoke(m_func, m_index++, std::forward<Args>(args)...))
        {
            return std::invoke(m_func, m_index++, std::forward<Args>(args)...);
        }
    };

    template <class Func>
    auto operator()(Func&& func) const -> impl<std::decay_t<Func>>
    {
        return { std::forward<Func>(func) };
    }
};

static constexpr inline auto indexed_function = indexed_function_fn{};

}  // namespace detail
}  // namespace seq
}  // namespace ferrugo