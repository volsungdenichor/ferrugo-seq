#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <ferrugo/core/std_ostream.hpp>
#include <ferrugo/seq/seq.hpp>

#include "matchers.hpp"

using namespace ferrugo;
using namespace std::string_literals;

TEST_CASE("constructor", "[sequence]")
{
    const auto fn = []() mutable -> core::optional<int> { return {}; };
    seq::sequence<int> s{ fn };
    REQUIRE_THAT(s, matchers::is_empty());
}

TEST_CASE("range", "[sequence][initializers]")
{
    REQUIRE_THAT(seq::range(0, 10), matchers::elements_are(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    REQUIRE_THAT(seq::range(5, 10), matchers::elements_are(5, 6, 7, 8, 9));
}

TEST_CASE("range - lower > upper", "[sequence][initializers]")
{
    REQUIRE_THAT(seq::range(10, 0), Catch::Matchers::IsEmpty());
}

TEST_CASE("range - linspace", "[sequence][initializers]")
{
    REQUIRE_THAT(seq::linspace(2.F, 5.F, 4), matchers::elements_are(2.F, 3.F, 4.F, 5.F));
}

TEST_CASE("transform", "[sequence]")
{
    REQUIRE_THAT(
        seq::range(0, 10) |= seq::transform([](int x) { return std::to_string(x * 10); }),
        matchers::elements_are("0"s, "10"s, "20"s, "30"s, "40"s, "50"s, "60"s, "70"s, "80"s, "90"s));
}

TEST_CASE("transform_i", "[sequence][indexed]")
{
    REQUIRE_THAT(
        seq::range(0, 10) |= seq::transform_i([](int i, int x) { return std::to_string(i + x * 10); }),
        matchers::elements_are("0"s, "11"s, "22"s, "33"s, "44"s, "55"s, "66"s, "77"s, "88"s, "99"s));
}

TEST_CASE("transform_i - multiple pass", "[sequence][indexed]")
{
    const auto op = seq::transform_i([](int i, int x) { return std::to_string(i + x * 10); });
    REQUIRE_THAT(seq::range(5) |= op, matchers::elements_are("0"s, "11"s, "22"s, "33"s, "44"s));
    REQUIRE_THAT(seq::range(5) |= op, matchers::elements_are("0"s, "11"s, "22"s, "33"s, "44"s));
}

TEST_CASE("transform_maybe", "[sequence]")
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

TEST_CASE("transform_maybe_i", "[sequence][indexed]")
{
    const auto f = [](int i, int x) -> core::optional<std::string>
    {
        if (i % 3 == 0)
        {
            return "_" + std::to_string(i);
        }
        if (x % 2 == 0)
        {
            return std::to_string(x);
        }
        return {};
    };
    REQUIRE_THAT(
        seq::range(0, 10) |= seq::transform_maybe_i(f),
        matchers::elements_are("_0"s, "2"s, "_3"s, "4"s, "_6"s, "8"s, "_9"s));
}

TEST_CASE("transform_maybe_i - multiple pass", "[sequence][indexed]")
{
    const auto f = [](int i, int x) -> core::optional<std::string>
    {
        if (i % 3 == 0)
        {
            return "_" + std::to_string(i);
        }
        if (x % 2 == 0)
        {
            return std::to_string(x);
        }
        return {};
    };
    const auto op = seq::transform_maybe_i(f);
    REQUIRE_THAT(seq::range(0, 5) |= op, matchers::elements_are("_0", "2", "_3", "4"));
    REQUIRE_THAT(seq::range(0, 5) |= op, matchers::elements_are("_0", "2", "_3", "4"));
}

TEST_CASE("filter", "[sequence]")
{
    REQUIRE_THAT(seq::range(0, 10) |= seq::filter([](int x) { return x % 3 == 0; }), matchers::elements_are(0, 3, 6, 9));
}

TEST_CASE("filter_i", "[sequence]")
{
    REQUIRE_THAT(
        seq::range(0, 10) |= seq::filter_i([](int i, int x) { return i % 2 == 0 && x % 3 == 0; }),
        matchers::elements_are(0, 6));
}

TEST_CASE("filter_i - multiple pass", "[sequence][indexed]")
{
    const auto op = seq::filter_i([](int i, int x) { return i % 3 == 0; });
    REQUIRE_THAT(seq::range(0, 10) |= op, matchers::elements_are(0, 3, 6, 9));
    REQUIRE_THAT(seq::range(0, 10) |= op, matchers::elements_are(0, 3, 6, 9));
}

TEST_CASE("take", "[sequence]")
{
    REQUIRE_THAT(seq::range(0, 10) |= seq::take(5), matchers::elements_are(0, 1, 2, 3, 4));
}

TEST_CASE("drop", "[sequence]")
{
    REQUIRE_THAT(seq::range(0, 10) |= seq::drop(5), matchers::elements_are(5, 6, 7, 8, 9));
}

TEST_CASE("step", "[sequence]")
{
    REQUIRE_THAT(seq::range(0, 10) |= seq::step(3), matchers::elements_are(0, 3, 6, 9));
}

TEST_CASE("drop_while", "[sequence]")
{
    REQUIRE_THAT(seq::range(0, 10) |= seq::drop_while([](int x) { return x < 5; }), matchers::elements_are(5, 6, 7, 8, 9));
}

TEST_CASE("take_while", "[sequence]")
{
    REQUIRE_THAT(seq::range(0, 10) |= seq::take_while([](int x) { return x < 5; }), matchers::elements_are(0, 1, 2, 3, 4));
}

TEST_CASE("zip", "[sequence]")
{
    REQUIRE_THAT(
        seq::zip(seq::range(10, 15), seq::range(100, 110)),
        matchers::elements_are(
            std::tuple{ 10, 100 },
            std::tuple{ 11, 101 },
            std::tuple{ 12, 102 },
            std::tuple{ 13, 103 },
            std::tuple{ 14, 104 }));
}

TEST_CASE("zip_transform", "[sequence]")
{
    REQUIRE_THAT(
        seq::zip_transform(std::plus<>{}, seq::range(10, 15), seq::range(100, 110)),
        matchers::elements_are(110, 112, 114, 116, 118));
}

TEST_CASE("chain", "[sequence]")
{
    REQUIRE_THAT(
        seq::chain(seq::range(0, 5), seq::range(100, 105)), matchers::elements_are(0, 1, 2, 3, 4, 100, 101, 102, 103, 104));
}

TEST_CASE("join", "[sequence]")
{
    REQUIRE_THAT(
        seq::range(5) |= seq::transform([](int x) { return seq::range(x); }) |= seq::join(),
        matchers::elements_are(0, 0, 1, 0, 1, 2, 0, 1, 2, 3));
}

TEST_CASE("transform_join", "[sequence]")
{
    REQUIRE_THAT(
        seq::range(5) |= seq::transform_join([](int x) { return seq::range(x); }),
        matchers::elements_are(0, 0, 1, 0, 1, 2, 0, 1, 2, 3));
}

TEST_CASE("enumerate", "[sequence]")
{
    REQUIRE_THAT(
        seq::range(10, 15) |= seq::enumerate(0),
        matchers::elements_are(
            std::tuple{ 0, 10 }, std::tuple{ 1, 11 }, std::tuple{ 2, 12 }, std::tuple{ 3, 13 }, std::tuple{ 4, 14 }));
}

TEST_CASE("init", "[sequence][initializers]")
{
    REQUIRE_THAT(seq::init(6, [](int i) { return 10 * i + 1; }), matchers::elements_are(1, 11, 21, 31, 41, 51));
}

TEST_CASE("init_infinite", "[sequence][initializers]")
{
    REQUIRE_THAT(
        seq::init_infinite([](int i) { return 10 * i + 1; }) |= seq::take(10),
        matchers::elements_are(1, 11, 21, 31, 41, 51, 61, 71, 81, 91));
}

TEST_CASE("unfold", "[sequence][initializers]")
{
    using state_t = std::pair<int, int>;
    REQUIRE_THAT(
        seq::unfold(
            state_t{ 1, 1 },
            [](const state_t& s) -> core::optional<std::tuple<int, state_t>>
            {
                const auto [prev, cur] = s;
                if (cur > 20)
                {
                    return {};
                }
                return std::tuple<int, state_t>{ cur, state_t{ cur + prev, prev } };
            }),
        matchers::elements_are(1, 1, 2, 3, 5, 8, 13));
}

TEST_CASE("empty", "[sequence][initializers]")
{
    REQUIRE_THAT(seq::empty<int>(), matchers::is_empty());
}

TEST_CASE("view", "[sequence]")
{
    std::vector<int> v = { 2, 4, 9, 99, -1 };
    REQUIRE_THAT(seq::view(v), matchers::elements_are(2, 4, 9, 99, -1));
}

TEST_CASE("owning", "[sequence]")
{
    REQUIRE_THAT(seq::owning(std::vector<int>{ 2, 4, 9, 99, -1 }), matchers::elements_are(2, 4, 9, 99, -1));
    const seq::sequence<int> s = seq::owning(std::vector<int>{ 2, 4, 9, 99, -1 });
    REQUIRE_THAT(s, matchers::elements_are(2, 4, 9, 99, -1));
}

TEST_CASE("vec", "[sequence]")
{
    REQUIRE_THAT(seq::vec(2, 4, 9, 99, -1), matchers::elements_are(2, 4, 9, 99, -1));
}

TEST_CASE("maybe_front", "[sequence][terminals]")
{
    REQUIRE_THAT(bool(seq::empty<int>() |= seq::maybe_front), matchers::equal_to(false));
    REQUIRE_THAT(*(seq::repeat(3) |= seq::maybe_front), matchers::equal_to(3));
}

TEST_CASE("nth", "[sequence][terminals]")
{
    REQUIRE_THAT(*(seq::range(10) |= seq::nth(3)), matchers::equal_to(3));
}

TEST_CASE("find_if", "[sequence][terminals]")
{
    REQUIRE_THAT(bool(seq::range(10) |= seq::find_if([](int x) { return x > 100; })), matchers::equal_to(false));
    REQUIRE_THAT(*(seq::range(10) |= seq::find_if([](int x) { return x > 5; })), matchers::equal_to(6));
}

TEST_CASE("all_of", "[sequence][terminals]")
{
    REQUIRE_THAT(seq::range(10) |= seq::all_of([](int x) { return x >= 0; }), matchers::equal_to(true));
    REQUIRE_THAT(seq::range(10) |= seq::all_of([](int x) { return x >= 5; }), matchers::equal_to(false));
    REQUIRE_THAT(seq::range(10) |= seq::all_of([](int x) { return x < 20; }), matchers::equal_to(true));
}

TEST_CASE("any_of", "[sequence][terminals]")
{
    REQUIRE_THAT(seq::range(10) |= seq::any_of([](int x) { return x >= 0; }), matchers::equal_to(true));
    REQUIRE_THAT(seq::range(10) |= seq::any_of([](int x) { return x >= 5; }), matchers::equal_to(true));
    REQUIRE_THAT(seq::range(10) |= seq::any_of([](int x) { return x > 20; }), matchers::equal_to(false));
}

TEST_CASE("for_each", "[sequence][terminals]")
{
    std::stringstream buffer;
    const auto append = [&](int x) { buffer << " " << x; };
    seq::range(10) |= seq::for_each(append);
    REQUIRE_THAT(buffer.str(), matchers::equal_to(" 0 1 2 3 4 5 6 7 8 9"));
}

TEST_CASE("for_each_i", "[sequence][terminals][indexed]")
{
    std::stringstream buffer;
    const auto append = [&](int i, int x) { buffer << " (" << i << ") " << x; };
    seq::range(5) |= seq::for_each_i(append);
    REQUIRE_THAT(buffer.str(), matchers::equal_to(" (0) 0 (1) 1 (2) 2 (3) 3 (4) 4"));
}

TEST_CASE("fold", "[sequence][terminals]")
{
    REQUIRE_THAT(seq::range(10) |= seq::fold(0, std::plus<>{}), matchers::equal_to(45));
}

TEST_CASE("copy", "[sequence][terminals]")
{
    std::vector<int> out{ -1, -2, -3 };
    seq::range(5) |= seq::copy(std::back_inserter(out));
    REQUIRE_THAT(out, matchers::elements_are(-1, -2, -3, 0, 1, 2, 3, 4));
}

TEST_CASE("index_of", "[sequence][terminals]")
{
    const auto pred = [](int x) { return x > 5; };
    REQUIRE_THAT(seq::range(10) |= seq::index_of(pred), matchers::equal_to(6));
}
