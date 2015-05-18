#include "./platform/UnitTestSupport.hpp"

#include <arbiter/SequenceArbiter.hpp>
#include <arbiter/details/NullErrorReportingPolicy.hpp>

#include <cstddef>
#include <deque>
#include <utility>

namespace {

    class MockErrorReportingPolicy : public arbiter::details::NullErrorReportingPolicy<std::size_t>
    {
    public:
        using LineSequencePair = std::pair<std::size_t, std::size_t>;
        using GapPair = std::pair<std::size_t, std::size_t>;

        void Gap(const std::size_t sequenceNumber, const std::size_t gapSize)
        {
            gaps_.emplace_back(sequenceNumber, gapSize);
        }

        void GapFill(const std::size_t sequenceNumber, const std::size_t length)
        {
            gapFills_.emplace_back(sequenceNumber, length);
        }

        void DuplicateOnLine(const std::size_t line, const std::size_t sequence)
        {
            duplicatesOnLine_.emplace_back(line, sequence);
        }

        const std::deque<GapPair>& gaps() const { return gaps_; }
        const std::deque<GapPair>& gapFills() const { return gapFills_; }
        const std::deque<LineSequencePair>& dups() const { return duplicatesOnLine_; }

    private:
        std::deque<LineSequencePair> duplicatesOnLine_;
        std::deque<GapPair> gaps_;
        std::deque<GapPair> gapFills_;
    };

    struct SingleLineTraits
    {
        static constexpr std::size_t FirstExpectedSequenceNumber() { return 0; }
        static constexpr std::size_t NumberOfLines() { return 1; } 
        static constexpr std::size_t HistoryDepth() { return 10; }

        using SequenceType = std::size_t;
        using ErrorReportingPolicy = MockErrorReportingPolicy;
    };

    TEST(verifySequenceArbiterInstantiation)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorPolicy);
    }

    TEST(verifySequenceArbiterHandlesHappyPath)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 9));  // next message rolls over history boundry.
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 12));
    }

    TEST(verifySequenceArbiterReportsDuplicatesOnLineCorrectly)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));

        CHECK(!arbiter.validate(0, 1)); // duplicate
        CHECK(!arbiter.validate(0, 3)); // duplicate

        auto& dups = errorPolicy.dups();
        REQUIRE CHECK_EQUAL(2U, dups.size());

        CHECK_EQUAL(0U, dups[0].first);
        CHECK_EQUAL(1U, dups[0].second);

        CHECK_EQUAL(0U, dups[1].first);
        CHECK_EQUAL(3U, dups[1].second);
    }

    TEST(verifySequenceArbiterReportsDuplicatesOnLineCorrectlyOnHistoryWrapAround)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 9));
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 11));

        CHECK(!arbiter.validate(0, 8));     // duplicate
        CHECK(!arbiter.validate(0, 7));     // duplicate

        CHECK(!arbiter.validate(0, 0));     // duplicate in unrecoverable area, discard no report.

        auto& dups = errorPolicy.dups();
        REQUIRE CHECK_EQUAL(2U, dups.size());

        CHECK_EQUAL(0U, dups[0].first);
        CHECK_EQUAL(8U, dups[0].second);

        CHECK_EQUAL(0U, dups[1].first);
        CHECK_EQUAL(7U, dups[1].second);
    }

    TEST(verifySequenceArbiterHandlesForwardGapCorrectly)
    {
        MockErrorReportingPolicy errorReporter;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorReporter);

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));

        // now we have a gap (2, 3, 4) are missing...
        CHECK(arbiter.validate(0, 5));

        // verify gap was reported
        auto& gaps = errorReporter.gaps();
        REQUIRE CHECK_EQUAL(1U, gaps.size());

        CHECK_EQUAL(2U, gaps[0].first);     // gap starts at 2
        CHECK_EQUAL(3U, gaps[0].second);    // gap is 3 messages

        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 9));      // next message rolls over history boundry.
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 11));
    }

    TEST(verifySequenceArbiterHandlesGapFillsCorrectly)
    {
        MockErrorReportingPolicy errorReporter;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorReporter);

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));

        // now we have a gap (2, 3, 4) are missing...
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));

        // fill the gaps
        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 3));

        // verify gap fills were reported.
        auto& gapFills = errorReporter.gapFills();
        REQUIRE CHECK_EQUAL(3U, gapFills.size());

        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 9));
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 11));
    }

    // ensure that a gap on the other side of the history boundry can be filled correctly.
    TEST(verifySequenceArbiterHandlesGapFillCorrectlyWhenGapFillGoesAcrossHistoryBoundry)
    {
        MockErrorReportingPolicy errorReporter;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorReporter);

        // first nearly fill up our buffer (depth 10).
        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 5));

        // sequence 6 - 9 are missing
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 11));

        // now we fill the gap which needs to wrap
        // around to the end of the history.
        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 9));
        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 7));
    }

    struct TwoLineTraits
    {
        static constexpr std::size_t FirstExpectedSequenceNumber() { return 0; }
        static constexpr std::size_t NumberOfLines() { return 2; }
        static constexpr std::size_t HistoryDepth() { return 10; }

        using SequenceType = std::size_t;
        using ErrorReportingPolicy = MockErrorReportingPolicy;
    };

    TEST(verifySequenceArbiterHandlesHappyPathForTwoLines)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(1, 0));
        CHECK(arbiter.validate(1, 1));
        CHECK(!arbiter.validate(0, 0));
        CHECK(!arbiter.validate(0, 1));

        CHECK(arbiter.validate(0, 2));
        CHECK(!arbiter.validate(1, 2));

        CHECK(arbiter.validate(0, 3));
        CHECK(!arbiter.validate(1, 3));

        CHECK(arbiter.validate(1, 4));
        CHECK(arbiter.validate(1, 5));
        CHECK(arbiter.validate(1, 6));
        CHECK(arbiter.validate(1, 7));
        CHECK(!arbiter.validate(0, 4));
        CHECK(!arbiter.validate(0, 5));
        CHECK(!arbiter.validate(0, 6));
        CHECK(!arbiter.validate(0, 7));

        CHECK(arbiter.validate(0, 8));
        CHECK(!arbiter.validate(1,8));

        CHECK(arbiter.validate(1,9));   // next message rolls over history boundry.
        CHECK(arbiter.validate(1,10));
        CHECK(arbiter.validate(1,11));
        CHECK(arbiter.validate(1,12));

        CHECK(!arbiter.validate(0, 9));
        CHECK(!arbiter.validate(0, 10));
        CHECK(!arbiter.validate(0, 11));
        CHECK(!arbiter.validate(0, 12));
    }

    TEST(verifySequenceArbiterHandlesGapFillsCorrectlyWithTwoLines)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(!arbiter.validate(1, 0));

        CHECK(arbiter.validate(0, 1));
        CHECK(!arbiter.validate(1, 1));

        CHECK(arbiter.validate(0, 2));
        CHECK(!arbiter.validate(1, 2));

        // gap, missing 3, 4, 5
        CHECK(arbiter.validate(0, 6));
        CHECK(!arbiter.validate(1, 6));

        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 9));

        CHECK(arbiter.validate(1, 4));  // 0 is head
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(1, 5));

        // verify gaps reported correctly...
        auto& gaps = errorPolicy.gaps();
        REQUIRE CHECK_EQUAL(1U, gaps.size());

        CHECK_EQUAL(3U, gaps[0].first);
        CHECK_EQUAL(3U, gaps[0].second);

        // verify gap fills reported correctly...
        auto& gapFills = errorPolicy.gapFills();
        REQUIRE CHECK_EQUAL(3U, gapFills.size());

        CHECK_EQUAL(4U, gapFills[0].first);
        CHECK_EQUAL(3U, gapFills[1].first);
        CHECK_EQUAL(5U, gapFills[2].first);
    }
}
