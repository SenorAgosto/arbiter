#pragma once 
#include <arbiter/details/ArbiterCache.hpp>
#include <arbiter/details/ArbiterCacheAdvancer.hpp>

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

		SequenceArbiter();
		
		// Determine wether we should accept the @sequenceNumber or reject it
        inline bool validate(const std::size_t line, const SequenceType sequenceNumber);

        // Return SequenceArbiter to initial state.
		inline void reset();

	private:

		ErrorReportingPolicy errorPolicy_;  // TODO: move this to policy holder idiom

        details::ArbiterCache<Traits> cache_;
        details::ArbiterCacheAdvancer<Traits> advance_;
    };
	
	
	template<class Traits>
	SequenceArbiter<Traits>::SequenceArbiter()
        : advance_(cache_, errorPolicy_)
	{
    }

	template<class Traits>
	void SequenceArbiter<Traits>::reset()
	{
        cache_.reset();
        advance_.reset();
	}

	template<class Traits>
	bool SequenceArbiter<Traits>::validate(const std::size_t line, const SequenceType sequenceNumber)
	{
        return advance_(line, sequenceNumber);
    }
}
