#include <ferrugo/core/demangle.hpp>
#include <ferrugo/core/ostream_utils.hpp>
#include <ferrugo/core/pipe.hpp>
#include <ferrugo/core/std_ostream.hpp>
#include <ferrugo/seq/pipe.hpp>
#include <ferrugo/seq/seq.hpp>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

#define L(...) [](auto&& it) -> decltype((__VA_ARGS__)) { return (__VA_ARGS__); }

std::string trim(const std::string& text)
{
    static const auto is_space = [](char ch) { return std::isspace(ch); };
    const auto begin = std::find_if_not(text.begin(), text.end(), is_space);
    const auto end = std::find_if_not(text.rbegin(), text.rend(), is_space).base();
    return { begin, end };
}

int main()
{
    using namespace ferrugo;
    using namespace ferrugo::seq;

    std::string text = "Ala  ;  ma   ;   kota  ";
    std::stringstream is{ text };
    for (auto line : getlines(is, ';') |= transform(trim))
    {
        std::cout << std::quoted(line) << "\n";
    }

    // const std::vector<std::string> values = range('A', 'E') |= transform(core::str);

    // for (const auto&& [a, b, c, d] : cartesian_product(range(2), view(values), range(3), range(4)))
    // {
    //     std::cout << a << " " << b << " " << c << " " << d << "\n";
    // }
}
