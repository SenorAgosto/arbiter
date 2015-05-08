#pragma once 
#include <arbiter/details/LineSet.hpp>
#include <cstddef>

namespace arbiter { namespace details {

	// A structure to hold a count of sequence numbers
	template<typename SequenceType, std::size_t NumberOfLines>
	class SequenceInfo 
	{
	public:
        using LineSet = details::LineSet<NumberOfLines>;

		SequenceInfo();
		SequenceInfo(const SequenceType sequence);

        inline void insert(const std::size_t lineId);
        inline bool complete() const;

        inline const LineSet& lines() const;

		inline SequenceType sequence() const;
		inline void sequence(const SequenceType seq);

	private:
        LineSet lines_;
		SequenceType sequence_;
	};
	

	template<typename SequenceType, std::size_t NumberOfLines>
	SequenceInfo<SequenceType, NumberOfLines>::SequenceInfo()
        : sequence_()
	{
        lines_.fill();
	}
	
	template<typename SequenceType, std::size_t NumberOfLines>
	SequenceInfo<SequenceType, NumberOfLines>::SequenceInfo(const SequenceType sequence)
		: sequence_(sequence)
	{
	}

	template<typename SequenceType, std::size_t NumberOfLines>
	SequenceType SequenceInfo<SequenceType, NumberOfLines>::sequence() const
	{
		return sequence_;
	}
	
	template<typename SequenceType, std::size_t NumberOfLines>
	void SequenceInfo<SequenceType, NumberOfLines>::sequence(const SequenceType seq)
	{
		sequence_ = seq;
	}

    template<typename SequenceType, std::size_t NumberOfLines>
    bool SequenceInfo<SequenceType, NumberOfLines>::complete() const
    {
        return lines_.complete();
    }
}}
