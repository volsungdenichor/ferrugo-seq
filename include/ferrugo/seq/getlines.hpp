#pragma once

#include <ferrugo/seq/sequence.hpp>
#include <istream>
#include <string>

namespace ferrugo
{
namespace seq
{

namespace detail
{

struct getlines_fn
{
    struct next_function
    {
        std::istream& m_is;
        char m_delimiter;
        mutable std::string m_line = {};

        auto operator()() const -> maybe<std::string>
        {
            if (std::getline(m_is, m_line, m_delimiter))
            {
                return m_line;
            }
            return {};
        }
    };

    auto operator()(std::istream& is, char delimiter = '\n') const -> sequence<std::string>
    {
        return sequence<std::string>{ next_function{ is, delimiter } };
    }
};

}  // namespace detail

static constexpr inline auto getlines = detail::getlines_fn{};

}  // namespace seq
}  // namespace ferrugo