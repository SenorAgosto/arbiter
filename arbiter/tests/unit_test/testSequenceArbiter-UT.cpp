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

        void UnrecoverableGap(const std::size_t sequenceNumber, const std::size_t gapSize = 1)
        {
            unrecoverableGaps_.emplace_back(sequenceNumber, gapSize);
        }

		void LinePositionOverrun(const std::size_t slowLine, const std::size_t overrunByLine)
        {
            overruns_.emplace_back(slowLine, overrunByLine);
        }

        const std::deque<GapPair>& gaps() const { return gaps_; }
        const std::deque<GapPair>& gapFills() const { return gapFills_; }
        const std::deque<LineSequencePair>& dups() const { return duplicatesOnLine_; }
        const std::deque<GapPair>& unrecoverableGaps() const { return unrecoverableGaps_; }
        const std::deque<GapPair>& overruns() const { return overruns_; }

    private:
        std::deque<GapPair> unrecoverableGaps_;
        std::deque<LineSequencePair> duplicatesOnLine_;
        std::deque<GapPair> gaps_;
        std::deque<GapPair> gapFills_;
        std::deque<GapPair> overruns_;
    };

    struct SingleLineTraits
    {
        static constexpr std::size_t FirstExpectedSequenceNumber() { return 0; }
        static constexpr std::size_t LargestRecoverableGap() { return 5; }
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

    TEST(verifySequenceArbiterHandlesGapOnFirstSequenceNumber)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorPolicy);

        // gap 0 - 4
        CHECK(arbiter.validate(0, 5));

        // gap fill...
        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));

        CHECK(!arbiter.validate(0, 2));  // duplicate on line...
        CHECK(arbiter.validate(0, 6));

        auto& gaps = errorPolicy.gaps();
        REQUIRE CHECK_EQUAL(1U, gaps.size());

        CHECK_EQUAL(0U, gaps[0].first);
        CHECK_EQUAL(5U, gaps[0].second);

        auto& gapFills = errorPolicy.gapFills();
        REQUIRE CHECK_EQUAL(5U, gapFills.size());

        CHECK_EQUAL(4U, gapFills[0].first);
        CHECK_EQUAL(2U, gapFills[1].first);
        CHECK_EQUAL(3U, gapFills[2].first);
        CHECK_EQUAL(0U, gapFills[3].first);
        CHECK_EQUAL(1U, gapFills[4].first);

        auto& dups = errorPolicy.dups();
        REQUIRE CHECK_EQUAL(1U, dups.size());

        CHECK_EQUAL(0U, dups[0].first);     // duplicate on line 0
        CHECK_EQUAL(2U, dups[0].second);    // duplicate sequence 2
    }

    TEST(verifySequenceArbiterHandlesTooLargeOfGapOnFirstSequenceNumber)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorPolicy);

        // gap 0 - 6    (gap of length 7)
        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 8));

        // gap fill
        CHECK(!arbiter.validate(0, 0));     // unrecoverable
        CHECK(!arbiter.validate(0, 1));     // unrecoverable
        CHECK( arbiter.validate(0, 3));
        CHECK( arbiter.validate(0, 2));
        CHECK( arbiter.validate(0, 5));
        CHECK( arbiter.validate(0, 6));
        CHECK( arbiter.validate(0, 4));

        CHECK( arbiter.validate(0, 9));
        CHECK( arbiter.validate(0, 10));
        CHECK( arbiter.validate(0, 11));

        // verify unrecoverable reported...
        auto& unrecoverable = errorPolicy.unrecoverableGaps();
        REQUIRE CHECK_EQUAL(1U, unrecoverable.size());

        CHECK_EQUAL(0U, unrecoverable[0].first);
        CHECK_EQUAL(2U, unrecoverable[0].second);

        auto& gaps = errorPolicy.gaps();
        REQUIRE CHECK_EQUAL(1U, gaps.size());

        CHECK_EQUAL(2U, gaps[0].first);     // starting sequence of gap
        CHECK_EQUAL(5U, gaps[0].second);    // length of gap

        auto& gapFills = errorPolicy.gapFills();
        REQUIRE CHECK_EQUAL(5U, gapFills.size());

        CHECK_EQUAL(3U, gapFills[0].first);
        CHECK_EQUAL(2U, gapFills[1].first);
        CHECK_EQUAL(5U, gapFills[2].first);
        CHECK_EQUAL(6U, gapFills[3].first);
        CHECK_EQUAL(4U, gapFills[4].first);
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

    TEST(verifySequenceArbiterHandlesAGapThatsTooLarge)
    {
        MockErrorReportingPolicy errorReporter;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorReporter);

        CHECK(arbiter.validate(0, 0));

        // gap 1-7, (1, 2, 3) should be unrecoverable...
        CHECK(arbiter.validate(0, 8));

        CHECK(!arbiter.validate(0, 1));     // verify unrecoverable are lost...
        CHECK(!arbiter.validate(0, 2));

        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 5));

        auto& unrecoverable = errorReporter.unrecoverableGaps();
        REQUIRE CHECK_EQUAL(1U, unrecoverable.size());

        CHECK_EQUAL(1U, unrecoverable[0].first);
        CHECK_EQUAL(2U, unrecoverable[0].second);
    }

    TEST(verifySequenceArbiterHandlesAGapThatsTooLargeWhenGapGoesOverHistoryBoundry)
    {
        MockErrorReportingPolicy errorReporter;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorReporter);

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));

        // gap 5 - 11
        CHECK(arbiter.validate(0, 12));

        // these are unrecoverable
        CHECK(!arbiter.validate(0, 6));
        CHECK(!arbiter.validate(0, 5));

        CHECK(arbiter.validate(0, 11));
        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 9));
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

    TEST(verifySequenceArbiterResetsCorrectly)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));

        CHECK(!arbiter.validate(0, 0));
        CHECK(!arbiter.validate(0, 1));
        CHECK(!arbiter.validate(0, 2));
        CHECK(!arbiter.validate(0, 3));
        CHECK(!arbiter.validate(0, 4));

        arbiter.reset();

        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));

        CHECK(!arbiter.validate(0, 2));
        CHECK(!arbiter.validate(0, 3));
        CHECK(!arbiter.validate(0, 4));
        CHECK(!arbiter.validate(0, 1));
        CHECK(!arbiter.validate(0, 0));
    }

    struct TwoLineTraits
    {
        static constexpr std::size_t FirstExpectedSequenceNumber() { return 0; }
        static constexpr std::size_t LargestRecoverableGap() { return 5; }
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

    TEST(verifySequenceArbiterHandlesGapFillsCorrectlyWhenForwardGapOvertakesHeadWithTwoLines)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(!arbiter.validate(1, 0));

        CHECK(arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 2));

        // now we have a forward gap for [3,4,5] on line 1 which passes line 0 (current head)
        CHECK(arbiter.validate(1, 6));

        // fill the gap for 3 & 4...
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 4));

        auto& gaps = errorPolicy.gaps();
        REQUIRE CHECK_EQUAL(1U, gaps.size());

        CHECK_EQUAL(3U, gaps[0].first);
        CHECK_EQUAL(3U, gaps[0].second);

        auto& gapFills = errorPolicy.gapFills();
        REQUIRE CHECK_EQUAL(3U, gapFills.size());

        CHECK_EQUAL(3U, gapFills[0].first);
        CHECK_EQUAL(5U, gapFills[1].first);
        CHECK_EQUAL(4U, gapFills[2].first);
    }

    TEST(verifySequenceArbiterHandlesGapFillsCorrectlyWhenForwardGapOvertakesHeadWithTwoLines_2)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(!arbiter.validate(1, 0));

        CHECK(arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 2));

        // now we have a forward gap for [3,4,5] on line 1 which passes line 0 (current head)
        CHECK(arbiter.validate(1, 6));

        // fill the gap for 3 & 4...
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(1, 5));
        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(1, 7));

        auto& gaps = errorPolicy.gaps();
        REQUIRE CHECK_EQUAL(1U, gaps.size());

        CHECK_EQUAL(3U, gaps[0].first);
        CHECK_EQUAL(3U, gaps[0].second);

        auto& gapFills = errorPolicy.gapFills();
        REQUIRE CHECK_EQUAL(3U, gapFills.size());

        CHECK_EQUAL(3U, gapFills[0].first);
        CHECK_EQUAL(5U, gapFills[1].first);
        CHECK_EQUAL(4U, gapFills[2].first);
    }

    TEST(verifySequenceArbiterHandlesGapFillsCorrectlyWhenForwardGapOvertakesHeadWithTwoLines_3)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(!arbiter.validate(1, 0));

        CHECK(arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 2));

        // now we have a forward gap for [3,4,5] on line 1 which passes line 0 (current head)
        CHECK(arbiter.validate(1, 6));

        // fill the gap for 3 & 4...
        CHECK(arbiter.validate(1, 3));
        CHECK(arbiter.validate(1, 5));
        CHECK(arbiter.validate(1, 4));
        CHECK(arbiter.validate(0, 7));

        auto& gaps = errorPolicy.gaps();
        REQUIRE CHECK_EQUAL(1U, gaps.size());

        CHECK_EQUAL(3U, gaps[0].first);
        CHECK_EQUAL(3U, gaps[0].second);

        auto& gapFills = errorPolicy.gapFills();
        REQUIRE CHECK_EQUAL(3U, gapFills.size());

        CHECK_EQUAL(3U, gapFills[0].first);
        CHECK_EQUAL(5U, gapFills[1].first);
        CHECK_EQUAL(4U, gapFills[2].first);
    }

    TEST(verifySequenceArbiterHandlesGapFillsCorrectlyWhenForwardGapOvertakesHeadAndRollsOverHistoryBoundryWithTwoLines)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(!arbiter.validate(1, 0));

        CHECK(arbiter.validate(0, 1));
        CHECK(!arbiter.validate(1, 1));

        CHECK(arbiter.validate(0, 2));
        CHECK(!arbiter.validate(1, 2));

        CHECK(arbiter.validate(0, 3));
        CHECK(!arbiter.validate(1, 3));

        CHECK(arbiter.validate(0, 4));
        CHECK(!arbiter.validate(1, 4));

        CHECK(arbiter.validate(0, 5));
        CHECK(!arbiter.validate(1, 5));

        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));

        // gap [8, 9, 10, 11]
        CHECK(arbiter.validate(1, 12));
        CHECK(arbiter.validate(1, 13));

        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 9));
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 11));
        CHECK(!arbiter.validate(0, 12));
        CHECK(!arbiter.validate(0, 13));

        // verify gaps reported correctly...
        auto& gaps = errorPolicy.gaps();
        REQUIRE CHECK_EQUAL(1U, gaps.size());

        CHECK_EQUAL(8U, gaps[0].first);
        CHECK_EQUAL(4U, gaps[0].second);

        // verify gap fills reported correctly...
        auto& gapFills = errorPolicy.gapFills();
        REQUIRE CHECK_EQUAL(4U, gapFills.size());

        CHECK_EQUAL(8U, gapFills[0].first);
        CHECK_EQUAL(9U, gapFills[1].first);
        CHECK_EQUAL(10U, gapFills[2].first);
        CHECK_EQUAL(11U, gapFills[3].first);
    }

    TEST(verifySequenceArbiterHandlesGapFillsCorrectlyWhenForwardGapOvertakesHeadAndRollsOverHistoryBoundry_GapFilledOutOfOrder_WithTwoLines)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(!arbiter.validate(1, 0));

        CHECK(arbiter.validate(0, 1));
        CHECK(!arbiter.validate(1, 1));

        CHECK(arbiter.validate(0, 2));
        CHECK(!arbiter.validate(1, 2));

        CHECK(arbiter.validate(0, 3));
        CHECK(!arbiter.validate(1, 3));

        CHECK(arbiter.validate(0, 4));
        CHECK(!arbiter.validate(1, 4));

        CHECK(arbiter.validate(0, 5));
        CHECK(!arbiter.validate(1, 5));

        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));

        // gap [8, 9, 10, 11]
        CHECK(arbiter.validate(1, 12));
        CHECK(arbiter.validate(1, 13));

        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 9));
        CHECK(arbiter.validate(0, 11));
        CHECK(!arbiter.validate(0, 12));
        CHECK(!arbiter.validate(0, 13));

        // verify gaps reported correctly...
        auto& gaps = errorPolicy.gaps();
        REQUIRE CHECK_EQUAL(1U, gaps.size());

        CHECK_EQUAL(8U, gaps[0].first);
        CHECK_EQUAL(4U, gaps[0].second);

        // verify gap fills reported correctly...
        auto& gapFills = errorPolicy.gapFills();
        REQUIRE CHECK_EQUAL(4U, gapFills.size());

        CHECK_EQUAL(8U, gapFills[0].first);
        CHECK_EQUAL(10U, gapFills[1].first);
        CHECK_EQUAL(9U, gapFills[2].first);
        CHECK_EQUAL(11U, gapFills[3].first);
    }

    TEST(verifySequenceArbiterHandlesGapFillsCorrectlyWhenForwardGapOvertakesHeadAndRollsOverHistoryBoundry_GapFilledOutOfOrder_WithTwoLines_2)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(!arbiter.validate(1, 0));

        CHECK(arbiter.validate(0, 1));
        CHECK(!arbiter.validate(1, 1));

        CHECK(arbiter.validate(0, 2));
        CHECK(!arbiter.validate(1, 2));

        CHECK(arbiter.validate(0, 3));
        CHECK(!arbiter.validate(1, 3));

        CHECK(arbiter.validate(0, 4));
        CHECK(!arbiter.validate(1, 4));

        CHECK(arbiter.validate(0, 5));
        CHECK(!arbiter.validate(1, 5));

        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));

        // gap [8, 9, 10, 11]
        CHECK(arbiter.validate(1, 12));
        CHECK(arbiter.validate(1, 13));

        // gap fill
        CHECK(arbiter.validate(1, 8));
        CHECK(arbiter.validate(1, 10));
        CHECK(arbiter.validate(1, 9));
        CHECK(arbiter.validate(1, 11));
        CHECK(!arbiter.validate(0, 12));
        CHECK(!arbiter.validate(0, 13));

        // verify gaps reported correctly...
        auto& gaps = errorPolicy.gaps();
        REQUIRE CHECK_EQUAL(1U, gaps.size());

        CHECK_EQUAL(8U, gaps[0].first);
        CHECK_EQUAL(4U, gaps[0].second);

        // verify gap fills reported correctly...
        auto& gapFills = errorPolicy.gapFills();
        REQUIRE CHECK_EQUAL(4U, gapFills.size());

        CHECK_EQUAL(8U, gapFills[0].first);
        CHECK_EQUAL(10U, gapFills[1].first);
        CHECK_EQUAL(9U, gapFills[2].first);
        CHECK_EQUAL(11U, gapFills[3].first);
    }

    TEST(verifySequenceArbiterHandlesGapFillsCorrectlyWhenForwardGapOvertakesHeadAndRollsOverHistoryBoundry_GapFilledOutOfOrder_WithTwoLines_3)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(!arbiter.validate(1, 0));

        CHECK(arbiter.validate(0, 1));
        CHECK(!arbiter.validate(1, 1));

        CHECK(arbiter.validate(0, 2));
        CHECK(!arbiter.validate(1, 2));

        CHECK(arbiter.validate(0, 3));
        CHECK(!arbiter.validate(1, 3));

        CHECK(arbiter.validate(0, 4));
        CHECK(!arbiter.validate(1, 4));

        CHECK(arbiter.validate(0, 5));
        CHECK(!arbiter.validate(1, 5));

        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));

        // gap [8, 9, 10, 11]
        CHECK(arbiter.validate(1, 12));
        CHECK(arbiter.validate(1, 13));

        // gap fill...
        CHECK(arbiter.validate(1, 8));
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(1, 9));
        CHECK(arbiter.validate(0, 11));
        CHECK(!arbiter.validate(0, 12));
        CHECK(!arbiter.validate(0, 13));

        // verify gaps reported correctly...
        auto& gaps = errorPolicy.gaps();
        REQUIRE CHECK_EQUAL(1U, gaps.size());

        CHECK_EQUAL(8U, gaps[0].first);
        CHECK_EQUAL(4U, gaps[0].second);

        // verify gap fills reported correctly...
        auto& gapFills = errorPolicy.gapFills();
        REQUIRE CHECK_EQUAL(4U, gapFills.size());

        CHECK_EQUAL(8U, gapFills[0].first);
        CHECK_EQUAL(10U, gapFills[1].first);
        CHECK_EQUAL(9U, gapFills[2].first);
        CHECK_EQUAL(11U, gapFills[3].first);
    }

    TEST(verifySlowLineOverrunReported)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(1, 0));
        CHECK(arbiter.validate(1, 1));
        CHECK(arbiter.validate(1, 2));
        CHECK(arbiter.validate(1, 3));
        CHECK(arbiter.validate(1, 4));
        CHECK(arbiter.validate(1, 5));
        CHECK(arbiter.validate(1, 6));
        CHECK(arbiter.validate(1, 7));
        CHECK(arbiter.validate(1, 8));
        CHECK(arbiter.validate(1, 9));      // next messages rolls over and clobbers line 0's position.
        CHECK(arbiter.validate(1, 10));

        CHECK(!arbiter.validate(0, 0));     // ensure a slow line that starts sending data is handled correctly...
        CHECK(!arbiter.validate(0, 1));     //

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(1U, overruns.size());

        CHECK_EQUAL(0U, overruns[0].first);     // slow line 0
        CHECK_EQUAL(1U, overruns[0].second);    // overrun by line 1
    }

    TEST(verifySlowLineOverrunReported_2)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(!arbiter.validate(1, 0));
        CHECK(!arbiter.validate(1, 1));

        CHECK(arbiter.validate(1, 2));
        CHECK(arbiter.validate(1, 3));
        CHECK(arbiter.validate(1, 4));
        CHECK(arbiter.validate(1, 5));
        CHECK(arbiter.validate(1, 6));
        CHECK(arbiter.validate(1, 7));
        CHECK(arbiter.validate(1, 8));
        CHECK(arbiter.validate(1, 9));      // next message rolls over to position 0.
        CHECK(arbiter.validate(1, 10));
        CHECK(arbiter.validate(1, 11));     // this overruns line 0, line 0 gets bumped to next position.
        CHECK(arbiter.validate(1, 12));     // this overruns line 0, line 0 gets bumped to next position.

        CHECK(!arbiter.validate(0, 2));     // these positions were overrun, throw away the values
        CHECK(!arbiter.validate(0, 3));     // these positions were overrun, throw away the values

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(2U, overruns.size());

        CHECK_EQUAL(0U, overruns[0].first);     // slow line 0
        CHECK_EQUAL(1U, overruns[0].second);    // overrun by line 1

        CHECK_EQUAL(0U, overruns[0].first);     // slow line 0
        CHECK_EQUAL(1U, overruns[0].second);    // overrun by line 1
    }

    TEST(verifySlowLineOverrunReported_3)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(!arbiter.validate(1, 0));
        CHECK(!arbiter.validate(1, 1));

        CHECK(arbiter.validate(1, 2));
        CHECK(arbiter.validate(1, 3));
        CHECK(arbiter.validate(1, 4));
        CHECK(arbiter.validate(1, 5));
        CHECK(arbiter.validate(1, 6));
        CHECK(arbiter.validate(1, 7));
        CHECK(arbiter.validate(1, 8));
        CHECK(arbiter.validate(1, 9));      // next message rolls over to position 0.
        CHECK(arbiter.validate(1, 10));
        CHECK(arbiter.validate(1, 11));     // this overruns line 0, line 0 gets bumped to next position.
        CHECK(arbiter.validate(1, 12));     // this overruns line 0, line 0 gets bumped to next position.

        CHECK(!arbiter.validate(0, 4));     // these positions were overrun, coming in out of order.
        CHECK(!arbiter.validate(0, 3));
        CHECK(!arbiter.validate(0, 2));

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(2U, overruns.size());

        CHECK_EQUAL(0U, overruns[0].first);     // slow line 0
        CHECK_EQUAL(1U, overruns[0].second);    // overrun by line 1

        CHECK_EQUAL(0U, overruns[0].first);     // slow line 0
        CHECK_EQUAL(1U, overruns[0].second);    // overrun by line 1
    }

    TEST(verifySlowLineOverrunReportedWhenSlowLineStoppedAtEndOfHistory)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(!arbiter.validate(1, 0));

        CHECK(arbiter.validate(0, 1));
        CHECK(!arbiter.validate(1, 1));

        CHECK(arbiter.validate(0, 2));
        CHECK(!arbiter.validate(1, 2));

        CHECK(arbiter.validate(0, 3));
        CHECK(!arbiter.validate(1, 3));

        CHECK(arbiter.validate(0, 4));
        CHECK(!arbiter.validate(1, 4));

        CHECK(arbiter.validate(0, 5));
        CHECK(!arbiter.validate(1, 5));

        CHECK(arbiter.validate(0, 6));
        CHECK(!arbiter.validate(1, 6));

        CHECK(arbiter.validate(0, 7));
        CHECK(!arbiter.validate(1, 7));

        CHECK(arbiter.validate(0, 8));
        CHECK(!arbiter.validate(1, 8));

        CHECK(arbiter.validate(0, 9));
        CHECK(!arbiter.validate(1, 9));

        // now line 1 will stop sending... and line 0 will over take it.
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 11));
        CHECK(arbiter.validate(0, 12));
        CHECK(arbiter.validate(0, 13));
        CHECK(arbiter.validate(0, 14));
        CHECK(arbiter.validate(0, 15));
        CHECK(arbiter.validate(0, 16));
        CHECK(arbiter.validate(0, 17));
        CHECK(arbiter.validate(0, 18));
        CHECK(arbiter.validate(0, 19));     // overruns line 1
        CHECK(arbiter.validate(0, 20));     // overruns line 1

        CHECK(!arbiter.validate(1, 19));    // slow line comes back with already passed value.

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(2U, overruns.size());

        CHECK_EQUAL(1U, overruns[0].first);     // slow line 1
        CHECK_EQUAL(0U, overruns[0].second);    // overrun by line 0

        CHECK_EQUAL(1U, overruns[0].first);     // slow line 1
        CHECK_EQUAL(0U, overruns[0].second);    // overrun by line 0
    }

    TEST(verifySlowLineOverrunReportedWhenSlowLineStoppedAtEndOfHistory_2)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(!arbiter.validate(1, 0));

        CHECK(arbiter.validate(0, 1));
        CHECK(!arbiter.validate(1, 1));

        CHECK(arbiter.validate(0, 2));
        CHECK(!arbiter.validate(1, 2));

        CHECK(arbiter.validate(0, 3));
        CHECK(!arbiter.validate(1, 3));

        CHECK(arbiter.validate(0, 4));
        CHECK(!arbiter.validate(1, 4));

        CHECK(arbiter.validate(0, 5));
        CHECK(!arbiter.validate(1, 5));

        CHECK(arbiter.validate(0, 6));
        CHECK(!arbiter.validate(1, 6));

        CHECK(arbiter.validate(0, 7));
        CHECK(!arbiter.validate(1, 7));

        CHECK(arbiter.validate(0, 8));
        CHECK(!arbiter.validate(1, 8));

        CHECK(arbiter.validate(0, 9));
        CHECK(!arbiter.validate(1, 9));

        // now line 1 will stop sending... and line 0 will over take it.
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 11));
        CHECK(arbiter.validate(0, 12));
        CHECK(arbiter.validate(0, 13));
        CHECK(arbiter.validate(0, 14));
        CHECK(arbiter.validate(0, 15));
        CHECK(arbiter.validate(0, 16));
        CHECK(arbiter.validate(0, 17));
        CHECK(arbiter.validate(0, 18));
        CHECK(arbiter.validate(0, 19));     // overruns line 1
        CHECK(arbiter.validate(0, 20));     // overruns line 1

        CHECK(arbiter.validate(1, 21));     // slow line comes back with a sequence number in front of head...

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(2U, overruns.size());

        CHECK_EQUAL(1U, overruns[0].first);     // slow line 1
        CHECK_EQUAL(0U, overruns[0].second);    // overrun by line 0

        CHECK_EQUAL(1U, overruns[0].first);     // slow line 1
        CHECK_EQUAL(0U, overruns[0].second);    // overrun by line 0
    }

    TEST(verifySlowLineOverrunReported_4)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(1, 0));
        CHECK(arbiter.validate(1, 1));
        CHECK(arbiter.validate(1, 2));
        CHECK(arbiter.validate(1, 3));
        CHECK(arbiter.validate(1, 4));
        CHECK(arbiter.validate(1, 5));
        CHECK(arbiter.validate(1, 6));
        CHECK(arbiter.validate(1, 7));
        CHECK(arbiter.validate(1, 8));
        CHECK(arbiter.validate(1, 9));      // next messages rolls over and clobbers line 0's position.
        CHECK(arbiter.validate(1, 10));

        CHECK(!arbiter.validate(0, 1));     // the slow line comes back with a gapped value (0 missing).
        CHECK(!arbiter.validate(0, 2));

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(1U, overruns.size());

        CHECK_EQUAL(0U, overruns[0].first);     // slow line 0
        CHECK_EQUAL(1U, overruns[0].second);    // overrun by line 1
    }

    TEST(verifySlowLineOverrunReported_5)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(1, 0));
        CHECK(arbiter.validate(1, 1));
        CHECK(arbiter.validate(1, 2));
        CHECK(arbiter.validate(1, 3));
        CHECK(arbiter.validate(1, 4));
        CHECK(arbiter.validate(1, 5));
        CHECK(arbiter.validate(1, 6));
        CHECK(arbiter.validate(1, 7));
        CHECK(arbiter.validate(1, 8));
        CHECK(arbiter.validate(1, 9));      // next messages rolls over and clobbers line 0's position.
        CHECK(arbiter.validate(1, 10));

        CHECK(!arbiter.validate(0, 2));     // slow line sends values that should be dropped,
        CHECK(!arbiter.validate(0, 1));     // send is out of order

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(1U, overruns.size());

        CHECK_EQUAL(0U, overruns[0].first);     // slow line 0
        CHECK_EQUAL(1U, overruns[0].second);    // overrun by line 1
    }

    TEST(verifySlowLineOverrunReported_6)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(1, 0));
        CHECK(arbiter.validate(1, 1));
        CHECK(arbiter.validate(1, 2));
        CHECK(arbiter.validate(1, 3));
        CHECK(arbiter.validate(1, 4));
        CHECK(arbiter.validate(1, 5));
        CHECK(arbiter.validate(1, 6));
        CHECK(arbiter.validate(1, 7));
        CHECK(arbiter.validate(1, 8));
        CHECK(arbiter.validate(1, 9));      // next messages rolls over and clobbers line 0's position.
        CHECK(arbiter.validate(1, 10));

        CHECK(!arbiter.validate(0, 3));     // slow line startups again but gapped (0-2)
        CHECK(!arbiter.validate(0, 4));

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(1U, overruns.size());

        CHECK_EQUAL(0U, overruns[0].first);     // slow line 0
        CHECK_EQUAL(1U, overruns[0].second);    // overrun by line 1
    }

    TEST(verifySlowLineOverrunReported_7)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(!arbiter.validate(1, 0));
        CHECK(!arbiter.validate(1, 1));

        CHECK(arbiter.validate(1, 2));
        CHECK(arbiter.validate(1, 3));
        CHECK(arbiter.validate(1, 4));
        CHECK(arbiter.validate(1, 5));
        CHECK(arbiter.validate(1, 6));
        CHECK(arbiter.validate(1, 7));
        CHECK(arbiter.validate(1, 8));
        CHECK(arbiter.validate(1, 9));      // next message rolls over to position 0.
        CHECK(arbiter.validate(1, 10));
        CHECK(arbiter.validate(1, 11));     // this overruns line 0, line 0 gets bumped to next position.
        CHECK(arbiter.validate(1, 12));     // this overruns line 0, line 0 gets bumped to next position.

        CHECK(!arbiter.validate(0, 12));
        CHECK(arbiter.validate(0, 13));

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(2U, overruns.size());

        CHECK_EQUAL(0U, overruns[0].first);     // slow line 0
        CHECK_EQUAL(1U, overruns[0].second);    // overrun by line 1

        CHECK_EQUAL(0U, overruns[0].first);     // slow line 0
        CHECK_EQUAL(1U, overruns[0].second);    // overrun by line 1
    }

    TEST(verifySequenceArbiterHandlesSlowLineOverrunOnForwardGap)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<TwoLineTraits> arbiter(errorPolicy);

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(!arbiter.validate(1, 0));
        CHECK(!arbiter.validate(1, 1));

        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 8));

        // gap 9 - 14
        CHECK(arbiter.validate(0, 15));
        CHECK(arbiter.validate(0, 16));

        CHECK(!arbiter.validate(1, 2));     // unrecoverable
        CHECK(!arbiter.validate(1, 3));
        CHECK(!arbiter.validate(1, 4));
        CHECK(!arbiter.validate(1, 15));
        CHECK(arbiter.validate(1, 12));
        CHECK(!arbiter.validate(1, 7));
        CHECK(!arbiter.validate(1, 8));
        CHECK(arbiter.validate(1, 10));
        CHECK(arbiter.validate(1, 11));
        CHECK(!arbiter.validate(1, 9));     // 9 is unrecoverable
        CHECK(arbiter.validate(1, 13));
        CHECK(arbiter.validate(1, 14));
        CHECK(!arbiter.validate(1, 16));

        CHECK(arbiter.validate(1, 17));
        CHECK(!arbiter.validate(0, 17));

        CHECK(arbiter.validate(0, 18));
        CHECK(!arbiter.validate(1, 18));

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(1U, overruns.size());

        CHECK_EQUAL(1U, overruns[0].first);     // line 1 is overrun by
        CHECK_EQUAL(0U, overruns[0].second);    // line 0


        auto& unrecoverable = errorPolicy.unrecoverableGaps();
        REQUIRE CHECK_EQUAL(1U, unrecoverable.size());

        CHECK_EQUAL(9U, unrecoverable[0].first);
        CHECK_EQUAL(1U, unrecoverable[0].second);
    }

    struct NonZeroFirstExpectedSequenceNumber
    {
        static constexpr std::size_t FirstExpectedSequenceNumber() { return 4; }
        static constexpr std::size_t LargestRecoverableGap() { return 5; }
        static constexpr std::size_t NumberOfLines() { return 1; } 
        static constexpr std::size_t HistoryDepth() { return 10; }

        using SequenceType = std::size_t;
        using ErrorReportingPolicy = MockErrorReportingPolicy;
    };


    TEST(verifySequenceArbiterRejectsSequenceNumbersLessThanTheFirstExpectedSequenceNumber)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<NonZeroFirstExpectedSequenceNumber> arbiter(errorPolicy);

        CHECK(!arbiter.validate(0, 0));
        CHECK(!arbiter.validate(0, 1));
        CHECK(!arbiter.validate(0, 2));
        CHECK(!arbiter.validate(0, 3));

        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 6));
    }

    TEST(verifySequenceArbiterRejectsSequenceNumbersLessThanTheFirstExpectedSequenceNumberAfterAReset)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<NonZeroFirstExpectedSequenceNumber> arbiter(errorPolicy);

        CHECK(!arbiter.validate(0, 0));
        CHECK(!arbiter.validate(0, 1));
        CHECK(!arbiter.validate(0, 2));
        CHECK(!arbiter.validate(0, 3));

        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 6));

        arbiter.reset();

        CHECK(!arbiter.validate(0, 0));
        CHECK(!arbiter.validate(0, 1));
        CHECK(!arbiter.validate(0, 2));
        CHECK(!arbiter.validate(0, 3));

        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 6));
    }

    struct ThreeLineTraits
    {
        static constexpr std::size_t FirstExpectedSequenceNumber() { return 0; }
        static constexpr std::size_t LargestRecoverableGap() { return 5; }
        static constexpr std::size_t NumberOfLines() { return 3; }
        static constexpr std::size_t HistoryDepth() { return 10; }

        using SequenceType = std::size_t;
        using ErrorReportingPolicy = MockErrorReportingPolicy;
    };


    TEST(verifySlowLineOverrunForThreeLines)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<ThreeLineTraits> arbiter(errorPolicy);

        // position lines 1 & 2, then delay incoming messages until
        // line 0 overruns them.
        CHECK(arbiter.validate(1, 0));
        CHECK(!arbiter.validate(2, 0));
        CHECK(arbiter.validate(1, 1));
        CHECK(!arbiter.validate(2, 1));
        CHECK(arbiter.validate(2, 2));
        CHECK(!arbiter.validate(1, 2));

        // here comes the bull!
        CHECK(!arbiter.validate(0, 0));
        CHECK(!arbiter.validate(0, 1));
        CHECK(!arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 9));  // next message wraps in the history buffer
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 11));
        CHECK(arbiter.validate(0, 12)); // this overwrites slow lines: 1 & 2

        CHECK(arbiter.validate(0, 13)); // overwrites 1 & 2 again

        CHECK(arbiter.validate(1, 14));
        CHECK(arbiter.validate(2, 15));

        CHECK(!arbiter.validate(0, 14));
        CHECK(!arbiter.validate(0, 15));

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(5U, overruns.size());

        CHECK_EQUAL(1U, overruns[0].first);     // 1 is the slow line
        CHECK_EQUAL(0U , overruns[0].second);   // overrun by line 0

        CHECK_EQUAL(2U, overruns[1].first);     // 2 is the slow line
        CHECK_EQUAL(0U, overruns[1].second);    // overrun by line 0

        CHECK_EQUAL(1U, overruns[2].first);     // 1 is the slow line
        CHECK_EQUAL(0U, overruns[2].second);    // overrun by line 0

        CHECK_EQUAL(2U, overruns[3].first);     // 2 is the slow line
        CHECK_EQUAL(0U, overruns[3].second);    // overrun by line 0

        CHECK_EQUAL(2U, overruns[4].first);     // 2 is the slow line
        CHECK_EQUAL(1U, overruns[4].second);    // overrun by line 1
    }

    TEST(verifySlowLineOverrunForThreeLinesWhereOverrunIsByForwardGap)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<ThreeLineTraits> arbiter(errorPolicy);

        // position lines 1 & 2
        CHECK(arbiter.validate(1, 0));
        CHECK(!arbiter.validate(2, 0));
        CHECK(arbiter.validate(1, 1));
        CHECK(!arbiter.validate(2, 1));
        CHECK(arbiter.validate(1, 2));
        CHECK(!arbiter.validate(2, 2));
        CHECK(arbiter.validate(1, 3));
        CHECK(arbiter.validate(1, 4));

        // here comes the bull.
        CHECK(!arbiter.validate(0, 0));
        CHECK(!arbiter.validate(0, 1));
        CHECK(!arbiter.validate(0, 2));
        CHECK(!arbiter.validate(0, 3));
        CHECK(!arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 9));  // next message wraps in history buffer
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 11));

        // now we pass two slow lines with a forward gap
        CHECK(arbiter.validate(0, 15));

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(2U, overruns.size());

        CHECK_EQUAL(1U, overruns[0].first);     // line 1 is overrun
        CHECK_EQUAL(0U, overruns[0].second);    // by line 0

        CHECK_EQUAL(2U, overruns[1].first);     // line 2 is overrun
        CHECK_EQUAL(0U, overruns[1].second);    // by line 0
    }

    TEST(verifySlowLineOverrunForThreeLinesWhereOverrunWrapsAroundHistory)
    {
        MockErrorReportingPolicy errorPolicy;
        arbiter::SequenceArbiter<ThreeLineTraits> arbiter(errorPolicy);

        // position lines 1 & 2
        CHECK(arbiter.validate(1, 0));
        CHECK(!arbiter.validate(2, 0));     // leave line 2 at position 1
        CHECK(arbiter.validate(1, 1));      // leave line 1 at position 2

        // here comes the bull!
        CHECK(!arbiter.validate(0, 0));
        CHECK(!arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 9));      // next message wraps around history, but we're going to gap it.

        // gap for 10 & 11.
        CHECK(arbiter.validate(0, 12));

        CHECK(arbiter.validate(1, 13));
        CHECK(arbiter.validate(2, 14));

        auto& overruns = errorPolicy.overruns();
        REQUIRE CHECK_EQUAL(2U, overruns.size());

        CHECK_EQUAL(1U, overruns[0].first);     // line 1 is overrun
        CHECK_EQUAL(0U, overruns[0].second);    // by line 0

        CHECK_EQUAL(2U, overruns[1].first);     // line 2 is overrun
        CHECK_EQUAL(0U, overruns[1].second);    // by line 0

    }
}
