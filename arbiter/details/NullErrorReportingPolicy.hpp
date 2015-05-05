#pragma once 
#include <cstddef>

namespace arbiter { namespace details {

	template<typename SequenceType>
	struct NullErrorReportingPolicy 
	{
		void DuplicateOnLine(const std::size_t line, const SequenceType sequence){}
		
		void Gap(const SequenceType start, const SequenceType length){}
		void GapFill(const SequenceType start, const SequenceType length){}
		
		void LinePositionOverrun(const std::size_t slowLine, const std::size_t overrunByLine){}
		void UnrecoverableGap(const std::size_t line, const SequenceType start, const SequenceType length(){}	// reported if a sequence we're replacing has count of 0.
	};
}}
