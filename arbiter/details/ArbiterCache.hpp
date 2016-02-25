#pragma once
#include <arbiter/details/SequenceInfo.hpp>

#include <array>
#include <cstddef>
#include <limits>

namespace arbiter { namespace details {

    template<class Traits>
    struct ArbiterCache
    {
    public:
        using SequenceType = typename Traits::SequenceType;
        using SeqInfo = details::SequenceInfo<SequenceType, Traits::NumberOfLines()>;

        // verify Traits has these constexpr functions...
		static_assert(std::is_same<SequenceType, decltype(Traits::FirstExpectedSequenceNumber())>::value, "Traits::FirstExpectedSequenceNumber() has mismatched type. Type must be the same as SequenceType");
		static_assert(std::is_same<std::size_t, decltype(Traits::NumberOfLines())>::value, "Traits::NumberOfLines() doesn't return expected type.");
		static_assert(std::is_same<std::size_t, decltype(Traits::HistoryDepth())>::value, "Traits::HistoryDepth() doesn't return expected type.");

        ArbiterCache();

        void reset();
        std::size_t nextPosition(const std::size_t lineId); // return the next position in history for line.

    public:
		std::array<std::size_t, Traits::NumberOfLines()> positions;	// tracks where each line is in cache_.
		std::array<SeqInfo, Traits::HistoryDepth()> history;      // stores the sequence counts.

        std::size_t head;  // indicates the line which is ahead.
    };


    template<class Traits>
    ArbiterCache<Traits>::ArbiterCache()
        : head(std::numeric_limits<std::size_t>::max())
    {
        reset();
    }

    template<class Traits>
    void ArbiterCache<Traits>::reset()
    {
        head = std::numeric_limits<std::size_t>::max();

		for(auto& position : positions)
		{
			position = 0;
		}

        for(auto& historyValue : history)
        {
            historyValue = SeqInfo();
        }
    }

    template<class Traits>
    std::size_t ArbiterCache<Traits>::nextPosition(const std::size_t lineId)
    {
        return (positions[lineId] + 1) % history.size();
    }
}}
