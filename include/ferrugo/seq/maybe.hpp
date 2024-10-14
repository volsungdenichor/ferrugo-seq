#pragma once

#include <iostream>
#include <optional>

namespace ferrugo
{
namespace seq
{

struct none_t
{
    struct constructor_tag
    {
    };

    constexpr explicit none_t(constructor_tag)
    {
    }
};

static constexpr inline auto none = none_t{ none_t::constructor_tag{} };

template <class T>
class maybe;

template <class T>
class maybe
{
public:
    using value_type = T;
    using storage_type = std::optional<T>;

    constexpr maybe() noexcept = default;

    constexpr maybe(none_t) noexcept : maybe()
    {
    }

    template <class U, std::enable_if_t<std::is_constructible_v<value_type, U>, int> = 0>
    constexpr maybe(U&& value) : m_storage{ std::forward<U>(value) }
    {
    }

    template <class U, std::enable_if_t<std::is_constructible_v<value_type, U>, int> = 0>
    constexpr maybe(const maybe<U>& other) : m_storage{ other ? storage_type{ *other } : storage_type{} }
    {
    }

    template <class U, std::enable_if_t<std::is_constructible_v<value_type, U>, int> = 0>
    constexpr maybe(maybe<U>&& other) : m_storage{ other ? storage_type{ *std::move(other) } : storage_type{} }
    {
    }

    constexpr maybe(const maybe&) = default;
    constexpr maybe(maybe&&) noexcept = default;

    constexpr maybe& operator=(maybe other)
    {
        swap(other);
        return *this;
    }

    constexpr explicit operator bool() const noexcept
    {
        return static_cast<bool>(m_storage);
    }

    constexpr const value_type& operator*() const& noexcept
    {
        return *m_storage;
    }

    constexpr value_type& operator*() & noexcept
    {
        return *m_storage;
    }

    constexpr value_type&& operator*() && noexcept
    {
        return *std::move(m_storage);
    }

    constexpr const value_type* operator->() const& noexcept
    {
        return &**this;
    }

    constexpr value_type* operator->() & noexcept
    {
        return &**this;
    }

    constexpr value_type* operator->() && noexcept
    {
        return &**this;
    }

    constexpr void swap(maybe& other) noexcept
    {
        if constexpr (std::is_assignable_v<value_type, value_type>)
        {
            std::swap(m_storage, other.m_storage);
        }
        else
        {
            storage_type this_storage = std::move(m_storage);
            storage_type other_storage = std::move(other.m_storage);
            assign(m_storage, std::move(other_storage));
            assign(other.m_storage, std::move(this_storage));
        }
    }

private:
    static void assign(storage_type& item, storage_type&& value)
    {
        item.~storage_type();
        new (&item) storage_type{ std::move(value) };
    }

    storage_type m_storage;
};

template <class T>
class maybe<T&>
{
public:
    using value_type = T;
    using storage_type = T*;

    constexpr maybe() noexcept : m_storage{}
    {
    }

    constexpr maybe(none_t) noexcept : maybe()
    {
    }

    constexpr maybe(T& value) : m_storage{ &value }
    {
    }

    constexpr maybe(const maybe& other) = default;

    constexpr maybe(maybe&& other) noexcept = default;

    constexpr maybe& operator=(maybe other) noexcept
    {
        swap(other);
        return *this;
    }

    constexpr explicit operator bool() const noexcept
    {
        return static_cast<bool>(m_storage);
    }

    constexpr value_type& operator*() const noexcept
    {
        return *m_storage;
    }

    constexpr value_type* operator->() const noexcept
    {
        return m_storage;
    }

    constexpr void swap(maybe& other) noexcept
    {
        std::swap(m_storage, other.m_storage);
    }

private:
    storage_type m_storage;
};

template <class T>
std::ostream& operator<<(std::ostream& os, const maybe<T>& item)
{
    if (item)
    {
        return os << "some(" << *item << ")";
    }
    else
    {
        return os << "none";
    }
}

inline std::ostream& operator<<(std::ostream& os, const none_t&)
{
    return os << "none";
}

template <class L, class R, class = std::invoke_result_t<std::equal_to<>, L, R>>
constexpr bool operator==(const maybe<L>& lhs, const maybe<R>& rhs)
{
    return (!lhs && !rhs) || (lhs && rhs && *lhs == *rhs);
}

template <class L, class R, class = std::invoke_result_t<std::equal_to<>, L, R>>
constexpr bool operator!=(const maybe<L>& lhs, const maybe<R>& rhs)
{
    return !(lhs == rhs);
}

template <class L, class R, class = std::invoke_result_t<std::equal_to<>, L, R>>
constexpr bool operator==(const maybe<L>& lhs, const R& rhs)
{
    return lhs && (*lhs == rhs);
}

template <class L, class R, class = std::invoke_result_t<std::equal_to<>, L, R>>
constexpr bool operator!=(const maybe<L>& lhs, const R& rhs)
{
    return !(lhs == rhs);
}

template <class L, class R, class = std::invoke_result_t<std::equal_to<>, L, R>>
constexpr bool operator==(const L& lhs, const maybe<R>& rhs)
{
    return rhs == lhs;
}

template <class L, class R, class = std::invoke_result_t<std::equal_to<>, L, R>>
constexpr bool operator!=(const L& lhs, const maybe<R>& rhs)
{
    return !(lhs == rhs);
}

template <class L>
constexpr bool operator==(const maybe<L>& lhs, none_t)
{
    return !static_cast<bool>(lhs);
}

template <class L>
constexpr bool operator!=(const maybe<L>& lhs, none_t)
{
    return static_cast<bool>(lhs);
}

template <class R>
constexpr bool operator==(none_t, const maybe<R>& rhs)
{
    return !static_cast<bool>(rhs);
}

template <class R>
constexpr bool operator!=(none_t, const maybe<R>& rhs)
{
    return static_cast<bool>(rhs);
}

template <class T>
struct maybe_underlying_type;

template <class T>
using maybe_underlying_type_t = typename maybe_underlying_type<T>::type;

template <class T>
struct maybe_underlying_type<maybe<T>>
{
    using type = T;
};

}  // namespace seq
}  // namespace ferrugo
