#pragma once
#include <cstddef>

namespace arbiter { namespace details {

    enum class ArbiterCacheAdvancerStateEnum : std::uint8_t
    {
        InitialState,       // initial state
        AdvanceHead,        // advance head by 1
        AdvanceLine,        // advance non-head by 1
        GapFill,            // backwards gap (any line)
        HeadForwardGapFill, // forward gap fill (head)
        LineForwardGapFill, // forward gap fill (non-head)

        NumberOfEntries
    };
}}
