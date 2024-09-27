#pragma once

#include <functional>
#include <tuple>

namespace ferrugo
{
namespace seq
{

template <class... Pipes>
struct pipe_t
{
    std::tuple<Pipes...> m_pipes;

    constexpr pipe_t(std::tuple<Pipes...> pipes) : m_pipes{ std::move(pipes) }
    {
    }

    constexpr pipe_t(Pipes... pipes) : pipe_t{ std::tuple<Pipes...>{ std::move(pipes)... } }
    {
    }

private:
    template <std::size_t I, class... Args>
    constexpr auto invoke(Args&&... args) const -> decltype(std::invoke(std::get<I>(m_pipes), std::forward<Args>(args)...))
    {
        return std::invoke(std::get<I>(m_pipes), std::forward<Args>(args)...);
    }

    template <std::size_t I, class... Args, std::enable_if_t<(I + 1) == sizeof...(Pipes), int> = 0>
    constexpr auto call(Args&&... args) const -> decltype(invoke<I>(std::forward<Args>(args)...))
    {
        return invoke<I>(std::forward<Args>(args)...);
    }

    template <std::size_t I, class... Args, std::enable_if_t<(I + 1) < sizeof...(Pipes), int> = 0>
    constexpr auto call(Args&&... args) const -> decltype(call<I + 1>(invoke<I>(std::forward<Args>(args)...)))
    {
        return call<I + 1>(invoke<I>(std::forward<Args>(args)...));
    }

public:
    template <class... Args>
    constexpr auto operator()(Args&&... args) const -> decltype(call<0>(std::forward<Args>(args)...))
    {
        return call<0>(std::forward<Args>(args)...);
    }
};

namespace detail
{

template <class T>
struct is_pipeline : std::false_type
{
};

template <class... Args>
struct is_pipeline<pipe_t<Args...>> : std::true_type
{
};

struct pipe_builder_fn
{
private:
    template <class Pipe>
    constexpr auto to_tuple(Pipe pipe) const -> std::tuple<Pipe>
    {
        return std::tuple<Pipe>{ std::move(pipe) };
    }

    template <class... Pipes>
    constexpr auto to_tuple(pipe_t<Pipes...> pipe) const -> std::tuple<Pipes...>
    {
        return pipe.m_pipes;
    }

    template <class... Pipes>
    constexpr auto from_tuple(std::tuple<Pipes...> tuple) const -> pipe_t<Pipes...>
    {
        return pipe_t<Pipes...>{ std::move(tuple) };
    }

public:
    template <class... Pipes>
    constexpr auto operator()(Pipes&&... pipes) const
        -> decltype(from_tuple(std::tuple_cat(to_tuple(std::forward<Pipes>(pipes))...)))
    {
        return from_tuple(std::tuple_cat(to_tuple(std::forward<Pipes>(pipes))...));
    }
};

}  // namespace detail

static constexpr inline auto pipe = detail::pipe_builder_fn{};
static constexpr inline auto fn = pipe;

template <class... L, class... R>
constexpr auto operator|=(pipe_t<L...> lhs, pipe_t<R...> rhs) -> decltype(pipe(std::move(lhs), std::move(rhs)))
{
    return pipe(std::move(lhs), std::move(rhs));
}

template <class T, class... Pipes, std::enable_if_t<!detail::is_pipeline<std::decay_t<T>>::value, int> = 0>
constexpr auto operator|=(T&& item, const pipe_t<Pipes...>& p) -> decltype(p(std::forward<T>(item)))
{
    return p(std::forward<T>(item));
}

}  // namespace seq
}  // namespace ferrugo
