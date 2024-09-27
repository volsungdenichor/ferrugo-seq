#pragma once

#include <ferrugo/seq/maybe.hpp>
#include <functional>
#include <limits>

namespace ferrugo
{

namespace seq
{

template <class T>
using next_function_t = std::function<maybe<T>()>;

template <class T>
struct empty_sequence
{
    auto operator()() const -> maybe<T>
    {
        return {};
    }
};

template <class To, class From>
struct cast_sequence
{
    next_function_t<From> m_from;

    auto operator()() const -> maybe<To>
    {
        maybe<From> value = m_from();
        if (value)
        {
            return static_cast<To>(*value);
        }
        return {};
    }
};

template <class T>
struct pointer_proxy
{
    T item;

    T* operator->()
    {
        return std::addressof(item);
    }
};

template <class T>
class sequence
{
public:
    using next_fn_t = next_function_t<T>;

    class iterator
    {
    public:
        using reference = T;
        using difference_type = std::ptrdiff_t;
        using value_type = std::decay_t<reference>;
        using pointer
            = std::conditional_t<std::is_reference_v<reference>, std::add_pointer_t<reference>, pointer_proxy<reference>>;
        using iterator_category = std::forward_iterator_tag;

        iterator() : m_next_fn{}, m_current{}, m_index{ std::numeric_limits<difference_type>::max() }
        {
        }

        iterator(const next_fn_t& next_fn) : m_next_fn{ std::move(next_fn) }, m_current{ m_next_fn() }, m_index{ 0 }
        {
        }

        iterator(const iterator&) = default;
        iterator(iterator&&) = default;

        iterator& operator=(iterator other)
        {
            std::swap(m_next_fn, m_next_fn);
            return *this;
        }

        reference operator*() const
        {
            return *m_current;
        }

        pointer operator->() const
        {
            if constexpr (std::is_reference_v<reference>)
            {
                return std::addressof(**this);
            }
            else
            {
                return pointer{ **this };
            }
        }

        iterator& operator++()
        {
            m_current = m_next_fn();
            ++m_index;
            return *this;
        }

        iterator operator++(int)
        {
            iterator temp{ *this };
            ++(*this);
            return temp;
        }

        friend bool operator==(const iterator& lhs, const iterator& rhs)
        {
            return (!lhs.m_current && !rhs.m_current) || (lhs.m_current && rhs.m_current && lhs.m_index == rhs.m_index);
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        next_fn_t m_next_fn;
        maybe<reference> m_current;
        difference_type m_index;
    };

    using value_type = typename iterator::value_type;
    using reference = typename iterator::reference;
    using pointer = typename iterator::pointer;

    explicit sequence(next_fn_t next_fn) : m_next_fn{ std::move(next_fn) }
    {
    }

    sequence() : sequence(empty_sequence<T>{})
    {
    }

    template <class U, std::enable_if_t<std::is_constructible_v<T, U>, int> = 0>
    sequence(const sequence<U>& other) : sequence(cast_sequence<T, U>{ other.get_next() })
    {
    }

    template <class Container, class = std::enable_if_t<std::is_constructible_v<Container, iterator, iterator>>>
    operator Container() const
    {
        return Container{ begin(), end() };
    }

    iterator begin() const
    {
        return iterator{ m_next_fn };
    }

    iterator end() const
    {
        return iterator{};
    }

    bool empty() const
    {
        return begin() == end();
    }

    const next_fn_t& get_next() const
    {
        return m_next_fn;
    }

    friend std::ostream& operator<<(std::ostream& os, const sequence& item)
    {
        // return os << "sequence<"
        //           << "???"
        //           << ">";
        os << "[";
        for (const auto& v : item)
        {
            os << " " << v;
        }
        os << " ]";
        return os;
    }

private:
    next_fn_t m_next_fn;
};

}  // namespace seq

}  // namespace ferrugo
