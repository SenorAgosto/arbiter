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

        inline void handleUnrecoverableForwardGap(ArbiterCacheAdvancerContext<Traits>& context, std::size_t& gapSize, SequenceType& currentSequenceNumber, const SequenceType& sequenceNumber);
        void checkForSlowLineOverrun(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const std::size_t position, const std::size_t gapSize);
        inline bool overrunsLine(const std::size_t position, const std::size_t linePosition, const std::size_t gapPosition, const std::size_t historySize);
    };

    template<class Traits>
    bool HeadForwardGapFill<Traits>::advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        auto& cache = context.cache;
        auto& positions = context.cache.positions;

        std::size_t position = positions[lineId];
        auto currentSequenceNumber = cache.history[position].sequence() + 1;
        position = cache.nextPosition(lineId);

        auto gapSize = sequenceNumber - currentSequenceNumber;
        handleUnrecoverableForwardGap(context, gapSize, currentSequenceNumber, sequenceNumber);

        context.errorPolicy.Gap(currentSequenceNumber, gapSize);

        auto gapPosition = positions[lineId] + gapSize;
        checkForSlowLineOverrun(context, lineId, position, gapPosition);

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
    void HeadForwardGapFill<Traits>::handleUnrecoverableForwardGap(ArbiterCacheAdvancerContext<Traits>& context, std::size_t& gapSize, SequenceType& currentSequenceNumber, const SequenceType& sequenceNumber)
    {
        if(gapSize > Traits::LargestRecoverableGap())
        {
            context.errorPolicy.UnrecoverableGap(currentSequenceNumber, gapSize - Traits::LargestRecoverableGap());

            gapSize = Traits::LargestRecoverableGap();
            currentSequenceNumber = sequenceNumber - Traits::LargestRecoverableGap();
        }
    }

    template<class Traits>
    void HeadForwardGapFill<Traits>::checkForSlowLineOverrun(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const std::size_t position, const std::size_t gapPosition)
    {
        auto& positions = context.cache.positions;

        std::size_t positionLineId = 0;
        for(auto& linePosition : positions)
        {
            if(positionLineId != lineId)
            {
                if(overrunsLine(position, linePosition, gapPosition, context.cache.history.size()))
                {
                    context.errorPolicy.LinePositionOverrun(positionLineId, lineId);
                    linePosition = (gapPosition + 1) % context.cache.history.size();
                }
            }

            ++positionLineId;
        }
    }

    template<class Traits>
    bool HeadForwardGapFill<Traits>::overrunsLine(const std::size_t position, const std::size_t linePosition, const std::size_t gapPosition, const std::size_t historySize)
    {
        return ((position <= linePosition) && (gapPosition >= linePosition)) ||
               ((position > linePosition) && (gapPosition >= (historySize + linePosition)));
    }
}}
