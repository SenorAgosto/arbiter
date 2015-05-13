#pragma once 
#include <arbiter/details/SequenceInfo.hpp>

#include <array>
#include <cstddef>

namespace arbiter { namespace details {

    template<class Traits>
    struct ArbiterCache
    {
        using SequenceType = typename Traits::SequenceType;
        using SequenceInfo = details::SequenceInfo<SequenceType, Traits::NumberOfLines()>;

        // verify Traits has these constexpr functions...
		static_assert(std::is_same<SequenceType, decltype(Traits::FirstExpectedSequenceNumber())>::value, "Traits::FirstExpectedSequenceNumber() has mismatched type. Type must be the same as SequenceType");
		static_assert(std::is_same<std::size_t, decltype(Traits::NumberOfLines())>::value, "Traits::NumberOfLines() doesn't return expected type.");
		static_assert(std::is_same<std::size_t, decltype(Traits::HistoryDepth())>::value, "Traits::HistoryDepth() doesn't return expected type.");

        ArbiterCache();
        void reset();

		std::array<std::size_t, Traits::NumberOfLines()> positions_;	// tracks where each line is in cache_.
		std::array<SequenceInfo, Traits::HistoryDepth()> history_;      // stores the sequence counts.

        std::size_t head_;  // indicates the line which is ahead.
    };


    template<class Traits>
    ArbiterCache<Traits>::ArbiterCache()
        : head_(0)
    {
        reset();
    }

    template<class Traits>
    void ArbiterCache<Traits>::reset()
    {
        head_ = 0;  // head line is line 0

		for(auto& position : positions_)
		{
			position = std::numeric_limits<std::size_t>::max();
		}

        for(auto& history : history_)
        {
            history = SequenceInfo();
        }
    }
}}
