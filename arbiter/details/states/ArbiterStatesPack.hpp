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
    // overload the operator[] for getting a reference to them.
    template<class Traits>
    class ArbiterStatesPack
    {
    public:
        ArbiterCacheAdvancerState<Traits>& operator[](ArbiterCacheAdvancerStateEnum state);

    private:
        InitialState<Traits> initialState_;
        AdvanceHead<Traits> advanceHead_;
        AdvanceLine<Traits> advanceLine_;
        GapFill<Traits> gapFill_;
        HeadForwardGapFill<Traits> headForwardGapFill_;
        LineForwardGapFill<Traits> lineForwardGapFill_;
    };

    template<class Traits>
    ArbiterCacheAdvancerState<Traits>& ArbiterStatesPack<Traits>::operator[](ArbiterCacheAdvancerStateEnum state)
    {
        // performance note: using a switch statement here turned
        // out to be faster than a vector-lookup with pointer dereference

        // order by frequency of occurance...
        switch(state)
        {
        case ArbiterCacheAdvancerStateEnum::AdvanceHead:
            return advanceHead_;
        case ArbiterCacheAdvancerStateEnum::AdvanceLine:
            return advanceLine_;
        case ArbiterCacheAdvancerStateEnum::GapFill:
            return gapFill_;
        case ArbiterCacheAdvancerStateEnum::HeadForwardGapFill:
            return headForwardGapFill_;
        case ArbiterCacheAdvancerStateEnum::LineForwardGapFill:
            return lineForwardGapFill_;
        case ArbiterCacheAdvancerStateEnum::InitialState:
            return initialState_;
        case ArbiterCacheAdvancerStateEnum::NumberOfEntries:
            break;  // fall through
        };

        throw ArbiterCacheAdvancerStateEnumOutOfRange(static_cast<std::size_t>(state));
    }
}}
