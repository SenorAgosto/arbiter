#pragma once 

#include <arbiter/details/states/InitialState.hpp>
#include <arbiter/details/states/AdvanceHead.hpp>
#include <arbiter/details/states/AdvanceLine.hpp>
#include <arbiter/details/states/GapFill.hpp>
#include <arbiter/details/states/HeadForwardGapFill.hpp>
#include <arbiter/details/states/LineForwardGapFill.hpp>

#include <arbiter/Exceptions.hpp>

namespace arbiter { namespace details {

    // Instantiate all the ArbiterCacheAdvancerStates and
    // implement the advance() interface.
    template<class Traits>
    class ArbiterStatesPack
    {
    public:
        using SequenceType = typename Traits::SequenceType;
        
        // perf: defining this method explicitly inline is giving the best performance
        bool advance(const ArbiterCacheAdvancerStateEnum state, ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber)
        {
            switch(state)
            {
                case ArbiterCacheAdvancerStateEnum::AdvanceHead:
                    return advanceHead_.advance(context, lineId, sequenceNumber);
                case ArbiterCacheAdvancerStateEnum::AdvanceLine:
                    return advanceLine_.advance(context, lineId, sequenceNumber);
                case ArbiterCacheAdvancerStateEnum::GapFill:
                    return gapFill_.advance(context, lineId, sequenceNumber);
                case ArbiterCacheAdvancerStateEnum::HeadForwardGapFill:
                    return headForwardGapFill_.advance(context, lineId, sequenceNumber);
                case ArbiterCacheAdvancerStateEnum::LineForwardGapFill:
                    return lineForwardGapFill_.advance(context, lineId, sequenceNumber);
                case ArbiterCacheAdvancerStateEnum::InitialState:
                    return initialState_.advance(context, lineId, sequenceNumber);
                case ArbiterCacheAdvancerStateEnum::NumberOfEntries:
                    break;  // fall through
            };
            
            throw ArbiterCacheAdvancerStateEnumOutOfRange(static_cast<std::size_t>(state));
        }
        
    private:
        InitialState<Traits> initialState_;
        AdvanceHead<Traits> advanceHead_;
        AdvanceLine<Traits> advanceLine_;
        GapFill<Traits> gapFill_;
        HeadForwardGapFill<Traits> headForwardGapFill_;
        LineForwardGapFill<Traits> lineForwardGapFill_;
    };
}}
