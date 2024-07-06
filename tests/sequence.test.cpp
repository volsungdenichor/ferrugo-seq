#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <ferrugo/seq/seq.hpp>

#include "matchers.hpp"

using namespace ferrugo;
using namespace std::string_literals;

TEST_CASE("sequence - constructor", "[sequence]")
{
    const auto fn = []() mutable -> core::optional<int> { return {}; };
    seq::sequence<int> s{ fn };
    REQUIRE_THAT(s, Catch::Matchers::IsEmpty());
}

TEST_CASE("range", "[sequence]")
{
    REQUIRE_THAT(seq::range(0, 10), matchers::elements_are(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    REQUIRE_THAT(seq::range(5, 10), matchers::elements_are(5, 6, 7, 8, 9));
}

TEST_CASE("range - lower > upper", "[sequence]")
{
    REQUIRE_THAT(seq::range(10, 0), Catch::Matchers::IsEmpty());
}

TEST_CASE("sequence - transform", "[sequence]")
{
    const auto f = [](int x) -> core::optional<std::string> { return std::to_string(x); };
    REQUIRE_THAT(
        seq::range(0, 10) |= seq::transform(f),
        matchers::elements_are("0"s, "1"s, "2"s, "3"s, "4"s, "5"s, "6"s, "7"s, "8"s, "9"s));
}

TEST_CASE("sequence - transform_maybe", "[sequence]")
{
    const auto f = [](int x) -> core::optional<std::string>
    {
        if (x % 2 == 0)
        {
            return std::to_string(x);
        }
        return {};
    };
    REQUIRE_THAT(seq::range(0, 10) |= seq::transform_maybe(f), matchers::elements_are("0"s, "2"s, "4"s, "6"s, "8"s));
}
