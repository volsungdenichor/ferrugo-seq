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

    seq::cartesian_product(seq::range(1, 20), seq::range(1, 20), seq::range(1, 100))
        |= seq::filter([](int a, int b, int c) { return a * a + b * b == c * c; })
        |= seq::for_each([](int a, int b, int c) { std::cout << a << " " << b << " " << c << "\n"; });
}
