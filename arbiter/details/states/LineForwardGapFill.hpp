#pragma once 
#include <arbiter/details/ArbiterCacheAdvancerContext.hpp>
#include <cstddef>

#include <arbiter/details/states/AdvanceHead.hpp>
#include <arbiter/details/states/HeadForwardGapFill.hpp>

namespace arbiter { namespace details {

    template<class Traits>
    class LineForwardGapFill
    {
    public:
        using SequenceType = typename Traits::SequenceType;
        bool advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber);

    private:
        bool handleHeadOverrun(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber);
        inline bool overrunsHead(const std::size_t headPosition, const std::size_t linePosition, const std::size_t gapPosition, const std::size_t historySize);
    };


    template<class Traits>
    bool LineForwardGapFill<Traits>::advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        auto& cache = context.cache;
        auto& positions = context.cache.positions;

        auto position = positions[lineId];
        auto headPosition = positions[cache.head];
        auto currentSequenceNumber = cache.history[position].sequence();

        auto gapPosition = (position + sequenceNumber - currentSequenceNumber);
        auto passesHead = overrunsHead(headPosition, position, gapPosition, cache.history.size());

        gapPosition %= cache.history.size();   // stay inbounds of history buffer

        if(passesHead)
        {
            return handleHeadOverrun(context, lineId, sequenceNumber);
        }

        auto sequenceMatch = sequenceNumber == cache.history[gapPosition].sequence();
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
        // this line becomes head...
        context.cache.positions[lineId] = context.cache.positions[context.cache.head];
        context.cache.head = lineId;

        auto nextSequenceNumber = context.cache.history[context.cache.positions[lineId]].sequence() + 1;
        bool isNext = sequenceNumber == nextSequenceNumber;

        if(isNext)
        {
            AdvanceHead<Traits> advancer;
            return advancer.advance(context, lineId, sequenceNumber);
        }

        HeadForwardGapFill<Traits> advancer;
        return advancer.advance(context, lineId, sequenceNumber);
    }

    // @gapPosition is not adjusted by historySize yet, it is monotonically increasing.
    template<class Traits>
    bool LineForwardGapFill<Traits>::overrunsHead(const std::size_t headPosition, const std::size_t linePosition, const std::size_t gapPosition, const std::size_t historySize)
    {
        // Replacing the following with likely more performant version to avoid a branch.
        //if(linePosition <= headPosition)
        //{
        //    return gapPosition > headPosition;
        //}

        //return gapPosition > (historySize + headPosition);

        return ((linePosition <= headPosition) && (gapPosition > headPosition)) ||
               ((linePosition >  headPosition) && (gapPosition > (historySize + headPosition)));
    }

}}
