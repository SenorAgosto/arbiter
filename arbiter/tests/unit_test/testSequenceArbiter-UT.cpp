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

        void DuplicateOnLine(const std::size_t line, const std::size_t sequence)
        {
            duplicatesOnLine_.emplace_back(line, sequence);
        }

        const std::deque<GapPair>& gaps() const
        {
            return gaps_;
        }

    private:
        std::deque<LineSequencePair> duplicatesOnLine_;
        std::deque<GapPair> gaps_;
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
        CHECK(arbiter.validate(0, 12));
    }
}
