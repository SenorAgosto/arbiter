#pragma once 
#include <arbiter/details/ArbiterCacheAdvancerState.hpp>

namespace arbiter { namespace details {

    template<class Traits>
    class AdvanceHead : public ArbiterCacheAdvancerState<Traits>
    {
    public:
        using ErrorReportingPolicy = typename Traits::ErrorReportingPolicy;
        using SequenceType = typename Traits::SequenceType;
        using SequenceInfo = SequenceInfo<SequenceType, Traits::NumberOfLines()>;

        bool advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber) override;

    private:
        void handleGaps(const SequenceInfo& sequenceInfo, ErrorReportingPolicy& errorPolicy);
    };


    template<class Traits>
    bool AdvanceHead<Traits>::advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        auto& cache = context.cache;

        auto nextPosition = cache.nextPosition(lineId);
        auto& sequenceInfo = cache.history[nextPosition];

        // TODO: check for slow line overrun...
        handleGaps(sequenceInfo, context.errorPolicy);

        cache.history[nextPosition] = SequenceInfo(lineId, sequenceNumber);
        cache.positions[lineId] = nextPosition;

        return true;    // new sequence number, accept the message
    }

    template<class Traits>
    void AdvanceHead<Traits>::handleGaps(const SequenceInfo& sequenceInfo, ErrorReportingPolicy& errorPolicy)
    {
        if(!sequenceInfo.complete())
        {
            if(sequenceInfo.empty())
            {
                errorPolicy.UnrecoverableGap(sequenceInfo.sequence());
            }
            else
            {
                const auto& missingLines = sequenceInfo.lines().missing();
                for(auto line : missingLines)
                {
                    errorPolicy.UnrecoverableLineGap(line, sequenceInfo.sequence());
                }
            }
        }
    }
}}
