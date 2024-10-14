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

int main()
{
    using namespace ferrugo;
    using namespace ferrugo::seq;

    seq::cartesian_product(seq::range(1, 20), seq::range(1, 20), seq::range(1, 100))
        |= seq::filter([](int a, int b, int c) { return a * a + b * b == c * c; })
        |= seq::for_each([](int a, int b, int c) { std::cout << a << " " << b << " " << c << "\n"; });

    // const std::vector<std::string> values = range('A', 'E') |= transform(core::str);

    // for (const auto&& [a, b, c, d] : cartesian_product(range(2), view(values), range(3), range(4)))
    // {
    //     std::cout << a << " " << b << " " << c << " " << d << "\n";
    // }
}
