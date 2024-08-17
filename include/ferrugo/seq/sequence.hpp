#pragma once

#include <ferrugo/core/iterator_interface.hpp>
#include <ferrugo/core/optional.hpp>
#include <ferrugo/core/pipe.hpp>
#include <ferrugo/core/range_interface.hpp>
#include <functional>
#include <limits>

namespace ferrugo
{
namespace seq
{

template <class T>
using next_function_t = std::function<core::optional<T>()>;

template <class T>
struct empty_sequence
{
    auto operator()() const -> core::optional<T>
    {
        return {};
    }
};

template <class To, class From>
struct cast_sequence
{
    next_function_t<From> m_from;

    auto operator()() const -> core::optional<To>
    {
        core::optional<From> value = m_from();
        if (value)
        {
            return *value;
        }
        return {};
    }
};

template <class T>
class sequence_base
{
public:
    using next_function_type = next_function_t<T>;

    struct iter
    {
        iter() : m_next{}, m_current{}, m_index{ std::numeric_limits<std::ptrdiff_t>::max() }
        {
        }

        iter(next_function_type next) : m_next{ std::move(next) }, m_current{ m_next() }, m_index{ 0 }
        {
        }

        T deref() const
        {
            return *m_current;
        }

        void inc()
        {
            m_current = m_next();
            ++m_index;
        }

        bool is_equal(const iter& other) const
        {
            return (!m_current && !other.m_current) || (m_current && other.m_current && m_index == other.m_index);
        }

        next_function_type m_next;
        core::optional<T> m_current;
        std::ptrdiff_t m_index;
    };

    using iterator = core::iterator_interface<iter>;

    explicit sequence_base(next_function_type next) : m_next(std::move(next))
    {
    }

    sequence_base() : sequence_base(empty_sequence<T>{})
    {
    }

    sequence_base(const sequence_base&) = default;
    sequence_base(sequence_base&&) = default;

    iterator begin() const
    {
        return iterator{ m_next };
    }

    iterator end() const
    {
        return iterator{};
    }

    bool empty() const
    {
        return begin() == end();
    }

    const next_function_type& get_next() const
    {
        return m_next;
    }

private:
    next_function_type m_next;
};

template <class T>
struct sequence : core::range_interface<sequence_base<T>>
{
    using base_type = core::range_interface<sequence_base<T>>;
    using base_type::base_type;

    template <class U, core::require<std::is_constructible<T, U>{}> = 0>
    sequence(sequence<U> other) : base_type(cast_sequence<T, U>{ other.get_next() })
    {
    }

    const typename sequence_base<T>::next_function_type& get_next() const
    {
        return base_type::get_impl().get_next();
    }
};

template <class T>
struct sequence_underlying_type;

template <class T>
using sequence_underlying_type_t = typename sequence_underlying_type<T>::type;

template <class T>
struct sequence_underlying_type<sequence<T>>
{
    using type = T;
};

}  // namespace seq
}  // namespace ferrugo
