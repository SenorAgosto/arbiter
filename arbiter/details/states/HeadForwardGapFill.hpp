#pragma once 
#include <arbiter/details/ArbiterCacheAdvancerState.hpp>

namespace arbiter { namespace details {

    template<class Traits>
    class HeadForwardGapFill : public ArbiterCacheAdvancerState<Traits>
    {
    public:
        using SequenceType = typename Traits::SequenceType;
        using SequenceInfo = SequenceInfo<SequenceType, Traits::NumberOfLines()>;

        bool advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber) override;

    private:
        void handleUnrecoverableForwardGap(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber);
        void reportGap(ArbiterCacheAdvancerContext<Traits>& context, const SequenceType currentSequenceNumber, const SequenceType endSequenceNumber);
    };

    template<class Traits>
    bool HeadForwardGapFill<Traits>::advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        handleUnrecoverableForwardGap(context, lineId, sequenceNumber);

        auto& cache = context.cache;
        auto& positions = context.cache.positions;

        std::size_t position = positions[lineId];
        auto currentSequenceNumber = cache.history[position].sequence() + 1;
        position = cache.nextPosition(lineId);

        const auto gapSize = sequenceNumber - currentSequenceNumber;
        context.errorPolicy.Gap(currentSequenceNumber, gapSize);

        while(currentSequenceNumber < sequenceNumber)
        {
            cache.history[position] = SequenceInfo(currentSequenceNumber++);

            cache.positions[lineId] = position;
            position = cache.nextPosition(lineId);
        }

        positions[lineId] = position;
        cache.history[position] = SequenceInfo(lineId, sequenceNumber);

        return true;
    }


    template<class Traits>
    void HeadForwardGapFill<Traits>::handleUnrecoverableForwardGap(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        // check to see if the gap is larger than our largest recoverable gap...
/*        auto currentSequenceNumber = context.cache.history[context.cache.head].sequence();

        std::ptrdiff_t gapSize = sequenceNumber - currentSequenceNumber;

        if(gapSize <= Traits::LargestRecoverableGapSize())
        {

        }
        else
        {
            // report some values as unrecoverable gaps
            context.errorPolicy.UnrecoverableGap();
        }
*/
    }

}}
