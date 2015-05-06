#include "./platform/UnitTestSupport.hpp"

#include <arbiter/SequenceArbiter.hpp>
#include <arbiter/details/NullErrorReportingPolicy.hpp>

#include <cstddef>

namespace {

    struct MockErrorReportingPolicy : public arbiter::details::NullErrorReportingPolicy<std::size_t>
    {
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
}
