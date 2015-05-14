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

        sequenceInfo.insert(lineId);
        cache.positions[lineId] = nextPosition;

        return false;   // we've seen the sequence number before...
    }
}}
