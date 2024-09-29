#include <ferrugo/core/demangle.hpp>
#include <ferrugo/core/ostream_utils.hpp>
#include <ferrugo/core/pipe.hpp>
#include <ferrugo/core/std_ostream.hpp>
#include <ferrugo/seq/pipe.hpp>
#include <ferrugo/seq/seq.hpp>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

#define L(...) [](auto&& it) -> decltype((__VA_ARGS__)) { return (__VA_ARGS__); }

int main()
{
    using namespace ferrugo;
    using namespace ferrugo::seq;

    const std::vector<std::string> values = range('A', 'E') |= transform(core::str);

    for (const auto item : cartesian_product(range(2), view(values), range(3), range(4)))
    {
        std::cout << item << "\n";
    }
}
