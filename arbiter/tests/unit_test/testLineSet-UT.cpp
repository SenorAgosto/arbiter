#include "./platform/UnitTestSupport.hpp"
#include <arbiter/details/LineSet.hpp>
#include <stdexcept>

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

    TEST(verifyCount)
    {
        arbiter::details::LineSet<2> set;
        CHECK_EQUAL(0U, set.count());

        set.insert(0);
        CHECK_EQUAL(1U, set.count());

        set.insert(1);
        CHECK_EQUAL(2U, set.count());
    }

    TEST(verifyComplete)
    {
        arbiter::details::LineSet<2> set;
        CHECK(!set.complete());

        set.insert(0);
        CHECK(!set.complete());

        set.insert(1);
        CHECK(set.complete());
    }

    TEST(verifyMissingLines)
    {
        arbiter::details::LineSet<3> set;
        auto missing = set.missing();

        /*REQUIRE*/ CHECK_EQUAL(3U, missing.size());
        CHECK_EQUAL(0U, missing[0]);
        CHECK_EQUAL(1U, missing[1]);
        CHECK_EQUAL(2U, missing[2]);

        set.insert(1);
        missing = set.missing();

        /*REQUIRE*/ CHECK_EQUAL(2U, missing.size());
        CHECK_EQUAL(0U, missing[0]);
        CHECK_EQUAL(2U, missing[1]);
    }

    TEST(verifyThrowsOnOutOfRange)
    {
        arbiter::details::LineSet<2> set;
        CHECK_THROW(set.insert(4), std::out_of_range);
    }
}
