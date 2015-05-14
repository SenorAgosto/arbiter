#pragma once 
#include <arbiter/details/ArbiterCache.hpp>
#include <arbiter/details/ArbiterCacheAdvancerContext.hpp>
#include <arbiter/details/ArbiterCacheAdvancerState.hpp>
#include <arbiter/details/ArbiterCacheAdvancerStateEnum.hpp>
#include <arbiter/details/states/ArbiterStatesPack.hpp>

#include <array>
#include <memory>

namespace arbiter { namespace details {

    // ArbiterCacheAdvancer is a state-pattern object used to assist
    // updating the arbiter cache values (which is a fairly complex
    // bit of logic).
    template<class Traits>
    class ArbiterCacheAdvancer
    {
    public:
        using ErrorReportingPolicy = typename Traits::ErrorReportingPolicy;
        using SequenceType = typename Traits::SequenceType;

        ArbiterCacheAdvancer(ArbiterCache<Traits>& cache, ErrorReportingPolicy& error);

        // advance the cache position for @lineId up to @sequenceNumber
        bool operator()(const std::size_t lineId, const SequenceType sequenceNumber);

    private:
        ArbiterCacheAdvancerStateEnum determineState(const std::size_t lineId, const SequenceType sequenceNumber);

    private:
        ArbiterStatesPack<Traits> states_;
        ArbiterCache<Traits>& cache_;
        ErrorReportingPolicy& errorPolicy_;

        ArbiterCacheAdvancerContext<Traits> context_;
        bool isFirstCall_;
    };


    template<class Traits>
    ArbiterCacheAdvancer<Traits>::ArbiterCacheAdvancer(ArbiterCache<Traits>& cache, ErrorReportingPolicy& error)
        : cache_(cache)
        , errorPolicy_(error)
        , context_(cache, error)
        , isFirstCall_(true)
    {
    }

    template<class Traits>
    bool ArbiterCacheAdvancer<Traits>::operator()(const std::size_t lineId, const SequenceType sequenceNumber)
    {
        ArbiterCacheAdvancerStateEnum state = determineState(lineId, sequenceNumber);
        return states_[state].advance(context_, lineId, sequenceNumber);
    }

    template<class Traits>
    ArbiterCacheAdvancerStateEnum ArbiterCacheAdvancer<Traits>::determineState(const std::size_t lineId, const SequenceType sequenceNumber)
    {
        const auto linePosition = cache_.positions[lineId];
        const auto currentSequenceNumber = cache_.history[linePosition].sequence();

        const bool isNext = currentSequenceNumber + 1 == sequenceNumber;
        const bool isHead = lineId == cache_.head;

        if(isFirstCall_)
        {
            isFirstCall_ = false;
            return ArbiterCacheAdvancerStateEnum::InitialState;
        }

        if(isNext)
        {
            return isHead ?
                ArbiterCacheAdvancerStateEnum::AdvanceHead :
                ArbiterCacheAdvancerStateEnum::AdvanceLine;
        }

        if(sequenceNumber <= currentSequenceNumber)
        {
            return ArbiterCacheAdvancerStateEnum::GapFill;
        }

        return isHead ?
            ArbiterCacheAdvancerStateEnum::HeadForwardGapFill :
            ArbiterCacheAdvancerStateEnum::LineForwardGapFill;
    }
}}
