#pragma once 
#include <arbiter/details/ArbiterCacheAdvancerState.hpp>

namespace arbiter { namespace details {

    template<class Traits>
    class AdvanceLine : public ArbiterCacheAdvancerState<Traits>
    {
    public:
        using SequenceType = typename Traits::SequenceType;

        bool advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber) override;
    };


    template<class Traits>
    bool AdvanceLine<Traits>::advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        auto& cache = context.cache;

        auto nextPosition = cache.nextPosition(lineId);
        auto& sequenceInfo = cache.history[nextPosition];

        bool sequenceMatch = sequenceNumber == sequenceInfo.sequence();
        bool accept = sequenceMatch && sequenceInfo.empty();

        if(accept)
        {
            context.errorPolicy.GapFill(sequenceNumber, 1);
        }
        else if(sequenceMatch && sequenceInfo.has(lineId))
        {
            context.errorPolicy.DuplicateOnLine(lineId, sequenceNumber);
        }

        sequenceInfo.insert(lineId);
        cache.positions[lineId] = nextPosition;

        return accept;
    }
}}
