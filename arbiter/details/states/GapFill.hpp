#pragma once 
#include <arbiter/details/ArbiterCacheAdvancerState.hpp>

namespace arbiter { namespace details {

    template<class Traits>
    class GapFill : public ArbiterCacheAdvancerState<Traits>
    {
    public:
        using SequenceType = typename Traits::SequenceType;
        using SequenceInfo = SequenceInfo<SequenceType, Traits::NumberOfLines()>;

        bool advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber) override;

    private:
        inline std::size_t calculateGapPosition(const std::size_t cacheSize, const std::size_t position, const SequenceType currentSequenceNumber, const SequenceType sequenceNumber);
    };


    template<class Traits>
    bool GapFill<Traits>::advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        auto& cache = context.cache;
        auto& positions = cache.positions;

        auto position = positions[lineId];
        auto currentSequenceNumber = cache.history[position].sequence();

        auto gapPosition = calculateGapPosition(cache.history.size(), position, currentSequenceNumber, sequenceNumber);

        if(sequenceNumber != cache.history[gapPosition].sequence())
        {
            // TODO: handle gap fills where we had
            // a large unrecoverable gap in-between...

            // binary search for the sequence number
        }

        auto accept = cache.history[gapPosition].empty();
        cache.history[gapPosition].insert(lineId);

        if(accept)
        {
            context.errorPolicy.GapFill(sequenceNumber, 1);
        }

        return accept;
    }

    template<class Traits>
    std::size_t GapFill<Traits>::calculateGapPosition(const std::size_t cacheSize, const std::size_t position, const SequenceType currentSequenceNumber, const SequenceType sequenceNumber)
    {
        const std::size_t sequenceDifference = currentSequenceNumber - sequenceNumber;

        if(sequenceDifference > position)
        {
            return cacheSize - (sequenceDifference - position);
        }

        return position - sequenceDifference;
    }
}}
