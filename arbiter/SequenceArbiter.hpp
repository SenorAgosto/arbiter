#pragma once 
#include <arbiter/details/SequenceInfo.hpp>

#include <array>
#include <cstddef>

namespace arbiter {

	// A sequence arbiter is used to ensure idempotence 
    // of messages in a stream by examining sequence numbers. 
	// Any expected duplicate messages are discarded. 
	// Any unexpected duplicate messages (e.g. duplicates on the
	// same line) are reported via the ErrorReportingPolicy.
	// The depth of history to keep is set via Traits. 
	// The history cache is treated like a circular buffer. 
	// If one line over-runs the position of another in the 
	// history cache by looping all the way around, a overrun error
	// will be reported and the lagging line's position moved forward.
	// If we find we're replacing sequences having gaps, an unrecoverable 
	// gap error will be reported. 
	template<class Traits>
	class SequenceArbiter 
	{
	public:
		using SequenceType = typename Traits::SequenceType;
		using ErrorReportingPolicy = typename Traits::ErrorReportingPolicy;
		
		// verify Traits has these constexpr functions...
		static_assert(std::is_same<SequenceType, decltype(Traits::FirstExpectedSequenceNumber())>::value, "Traits::FirstExpectedSequenceNumber() has mismatched type. Type must be the same as SequenceType");
		static_assert(std::is_same<std::size_t, decltype(Traits::NumberOfLines())>::value, "Traits::NumberOfLines() doesn't return expected type.");
		static_assert(std::is_same<std::size_t, decltype(Traits::HistoryDepth())>::value, "Traits::HistoryDepth() doesn't return expected type.");
		
		SequenceArbiter();
		
		// returns true if we should pass the message, false to discard.
		bool validate(const std::size_t line, const SequenceType sequenceNumber);
		
		// return all positions to starting state.
		void reset();
		
	private:
		using SequenceInfo = details::SequenceInfo<SequenceType>;
		
		std::array<std::size_t, Traits::NumberOfLines()> positions_;	// tracks where each line is in cache_.
		std::array<SequenceInfo, Traits::HistoryDepth()> cache_;	// stores the sequence counts

		ErrorReportingPolicy errorPolicy_;
	};
	
	
	template<class Traits>
	SequenceArbiter<Traits>::SequenceArbiter()
	{
		reset();

		for(auto& history : cache_)
		{
			history = SequenceInfo();
        }
	}
	
	template<class Traits>
	bool SequenceArbiter<Traits>::validate(const std::size_t line, const SequenceType sequenceNumber)
	{

		// TODO: implement...
		return false;
	}

	template<class Traits>
	void SequenceArbiter<Traits>::reset()
	{
		// TODO: reset first sequence number check
		// I.e. check to see if we have a gap on the first
		// recieved message.
		for(auto& position : positions_)
		{
			position = std::numeric_limits<std::size_t>::max();
		}
	}
}
