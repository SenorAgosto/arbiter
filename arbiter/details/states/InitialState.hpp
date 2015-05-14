#pragma once 
#include <arbiter/details/ArbiterCacheAdvancerState.hpp>

namespace arbiter { namespace details {

    template<class Traits>
    class InitialState : public ArbiterCacheAdvancerState<Traits>
    {
    public:
        using SequenceType = typename Traits::SequenceType;
        using SequenceInfo = SequenceInfo<SequenceType, Traits::NumberOfLines()>;

        bool advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber) override;
    };

    template<class Traits>
    bool InitialState<Traits>::advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        std::size_t position = 0;

        if(sequenceNumber != Traits::FirstExpectedSequenceNumber())
        {
            if(sequenceNumber < Traits::FirstExpectedSequenceNumber())
            {
                // report out of sequence
                return false;
            }
            else
            {
                // report the gap...
                //context.errorPolicy.Gap(0, sequenceNumber - 1);

                // for the current position to sequenceNumber, iterate through
                // the cache creating the correct gap SequenceInfos
            }
        }

        context.cache.history[position] = SequenceInfo(lineId, sequenceNumber);

        context.cache.positions[lineId] = position;
        context.cache.head = lineId;

        return true;
    }
}}
