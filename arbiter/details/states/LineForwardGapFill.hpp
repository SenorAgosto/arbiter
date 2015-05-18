#pragma once 
#include <arbiter/details/ArbiterCacheAdvancerState.hpp>
#include <arbiter/details/states/HeadForwardGapFill.hpp>

namespace arbiter { namespace details {

    template<class Traits>
    class LineForwardGapFill : public ArbiterCacheAdvancerState<Traits>
    {
    public:
        using SequenceType = typename Traits::SequenceType;
        bool advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber) override;

    private:
        bool handleHeadOverrun(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber);
    };


    template<class Traits>
    bool LineForwardGapFill<Traits>::advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        auto& cache = context.cache;
        auto& positions = context.cache.positions;

        auto position = positions[lineId];
        auto headPosition = positions[cache.head];

        auto currentSequenceNumber = cache.history[position].sequence();
        auto gapPosition = (position + sequenceNumber - currentSequenceNumber) % cache.history.size();
        auto sequenceMatch = sequenceNumber == cache.history[gapPosition].sequence();

        if(gapPosition > headPosition)
        {
            return handleHeadOverrun(context, lineId, sequenceNumber);
        }

        bool accept = sequenceMatch && cache.history[gapPosition].empty();
        if(accept)
        {
            context.errorPolicy.GapFill(sequenceNumber, 1);

            positions[lineId] = gapPosition;
            cache.history[gapPosition].insert(lineId);
        }
        else if(sequenceMatch)
        {
            positions[lineId] = gapPosition;
            cache.history[gapPosition].insert(lineId);
        }

        return accept;
    }

    template<class Traits>
    bool LineForwardGapFill<Traits>::handleHeadOverrun(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        return false;
    }
}}
