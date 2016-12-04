#pragma once
#include <arbiter/details/ArbiterCacheAdvancerContext.hpp>
#include <cstddef>

namespace arbiter { namespace details {

    template<class Traits>
    class InitialState
    {
    public:
        using SequenceType = typename Traits::SequenceType;
        using SeqInfo = SequenceInfo<SequenceType, Traits::NumberOfLines()>;

        bool advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber);

    private:
        bool handleInitialGap(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber);
    };


    template<class Traits>
    bool InitialState<Traits>::advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        std::size_t position = 0;

        if(sequenceNumber != Traits::FirstExpectedSequenceNumber())
        {
            return handleInitialGap(context, lineId, sequenceNumber);
        }

        context.cache.history[position] = SeqInfo(lineId, sequenceNumber);

        context.cache.positions[lineId] = position;
        context.cache.head = lineId;

        return true;
    }

    template<class Traits>
    bool InitialState<Traits>::handleInitialGap(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
    {
        if(sequenceNumber < Traits::FirstExpectedSequenceNumber())
        {
            context.isFirstCall = true;
            context.errorPolicy.FirstSequenceNumberOutOfSequence(lineId, sequenceNumber);
            return false;
        }

        auto nextSequenceNumber = Traits::FirstExpectedSequenceNumber();

        auto gapSize = sequenceNumber - Traits::FirstExpectedSequenceNumber();
        if(gapSize > Traits::LargestRecoverableGap())
        {
            auto unrecoverableLength = gapSize - Traits::LargestRecoverableGap();
            context.errorPolicy.UnrecoverableGap(Traits::FirstExpectedSequenceNumber(), unrecoverableLength);

            nextSequenceNumber += unrecoverableLength;
            gapSize -= unrecoverableLength;
        }

        auto& cache = context.cache;
        auto& positions = context.cache.positions;

        std::size_t position = 0;
        context.errorPolicy.Gap(nextSequenceNumber, gapSize);

        while(nextSequenceNumber < sequenceNumber)
        {
            cache.history[position] = SeqInfo(nextSequenceNumber++);
            positions[lineId] = position;

            position = cache.nextPosition(lineId);
        }

        positions[lineId] = position;

        cache.history[position] = SeqInfo(lineId, sequenceNumber);
        cache.head = lineId;

        return true;
    }
}}
