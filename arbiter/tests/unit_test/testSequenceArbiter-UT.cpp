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
        void DuplicateOnLine(const std::size_t line, const std::size_t sequence)
        {
            duplicatesOnLine_.emplace_back(line, sequence);
        }

    private:
        using LineSequencePair = std::pair<std::size_t, std::size_t>;
        std::deque<LineSequencePair> duplicatesOnLine_;
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
        arbiter::SequenceArbiter<SingleLineTraits> arbiter;
    }

    TEST(verifySequenceArbiterHandlesHappyPath)
    {
        arbiter::SequenceArbiter<SingleLineTraits> arbiter;

        CHECK(arbiter.validate(0, 0));
        CHECK(arbiter.validate(0, 1));
        CHECK(arbiter.validate(0, 2));
        CHECK(arbiter.validate(0, 3));
        CHECK(arbiter.validate(0, 4));
        CHECK(arbiter.validate(0, 5));
        CHECK(arbiter.validate(0, 6));
        CHECK(arbiter.validate(0, 7));
        CHECK(arbiter.validate(0, 8));
        CHECK(arbiter.validate(0, 9));  // next message rolls over
        CHECK(arbiter.validate(0, 10));
        CHECK(arbiter.validate(0, 12));
    }

/*
    TEST(verifySequenceArbiterFirstMessage)
    {
        static const std::size_t lineId = 0;
        arbiter::SequenceArbiter<SingleLineTraits> arbiter;

        CHECK(arbiter.validate(lineId, 0));
    }
*/
}
