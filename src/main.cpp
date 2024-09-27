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

    const std::vector<std::string> pl = range(26)  //
        |= drop_while(L(it < 13))                  //
        |= transform(core::str);

    const std::vector<std::string> en = { "Alice", "has", "cat", "and", "dog" };

    const auto f = [](int x) -> sequence<int> { return range(x); };

    range(5)             //
        |= transform(f)  //
        |= join          //
        |= for_each(L(std::cout << it << "\n"));
}
