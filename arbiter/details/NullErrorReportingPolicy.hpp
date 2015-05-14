#pragma once 
#include <cstddef>

namespace arbiter { namespace details {

	template<typename SequenceType>
	struct NullErrorReportingPolicy 
	{
		void DuplicateOnLine(const std::size_t /*line*/, const SequenceType /*sequence*/){}
		
		void Gap(const SequenceType /*start*/, const SequenceType /*length*/){}
		void GapFill(const SequenceType /*start*/, const SequenceType /*length*/){}

        // called when head overruns a slow line (history wrapped around)
		void LinePositionOverrun(const std::size_t /*slowLine*/, const std::size_t /*overrunByLine*/){}

        // called when we're overwriting a gap in the arbiter cache history.
		void UnrecoverableGap(const SequenceType /*start*/){}

        // called when we're overwriting a sequenceInfo in the cache history where 1 or more
        // lines didn't report, but at least 1 line did report (not a gap, but a line gap)
        void UnrecoverableLineGap(const std::size_t /*line*/, const SequenceType /*sequenceNumber*/){}
	};
}}
