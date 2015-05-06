#include "./platform/UnitTestSupport.hpp"
#include <arbiter/details/LineSet.hpp>

namespace {

    TEST(verifyInstantiation)
    {
        arbiter::details::LineSet<1> set;
    }

    TEST(verifyInsertion)
    {
        arbiter::details::LineSet<1> set;
        CHECK(!set[0]);

        set.insert(0);
        CHECK(set[0]);
    }

    TEST(verifyDuplicateIsReported)
    {
        arbiter::details::LineSet<1> set;
        CHECK(!set[0]);

        auto status = set.insert(0);
        CHECK(status);

        status = set.insert(0);
        CHECK(!status);
    }

    TEST(verifyMultipleLines)
    {
        arbiter::details::LineSet<2> set;

        CHECK(set.insert(0));
        CHECK(set.insert(1));

        CHECK(set[0]);
        CHECK(set[1]);
    }
}
