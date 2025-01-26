#include <ferrugo/seq/seq.hpp>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

int main()
{
    using namespace ferrugo;
    using namespace ferrugo::seq;

    const std::string text = "Ren";

    const auto res = seq::zip(  //
        seq::view(text),
        seq::view(text) |= seq::drop(1))
        |= seq::fold(
            0,
            [](int total, char x, char y) -> int
            {
                std::cerr << x << " " << y << "\n";
                if (x != y)
                {
                    return total + 1;
                }
                return total;
            });

    std::cout << res << "\n";
}
