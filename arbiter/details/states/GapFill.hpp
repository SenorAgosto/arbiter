#pragma once
#include <arbiter/details/ArbiterCacheAdvancerContext.hpp>
#include <cstddef>

namespace arbiter { namespace details {

    template<class Traits>
    class GapFill
    {
    public:
        using SequenceType = typename Traits::SequenceType;
        using SeqInfo = SequenceInfo<SequenceType, Traits::NumberOfLines()>;

        bool advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber);

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
        auto sequenceMatch = sequenceNumber == cache.history[gapPosition].sequence();
        auto accept = sequenceMatch && cache.history[gapPosition].empty();

        if(accept)
        {
            cache.history[gapPosition].insert(lineId);
            context.errorPolicy.GapFill(sequenceNumber, 1);
        }
        else if(sequenceMatch)
        {
            if(!cache.history[gapPosition].has(lineId))
            {
                cache.history[gapPosition].insert(lineId);
            }
            else
            {
                context.errorPolicy.DuplicateOnLine(lineId, sequenceNumber);
            }
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
