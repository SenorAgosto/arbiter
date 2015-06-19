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
		SequenceInfo(const std::size_t lineId, const SequenceType sequence);
        explicit SequenceInfo(const SequenceType sequence);

        inline void insert(const std::size_t lineId);
        inline bool has(const std::size_t lineId);

        inline bool complete() const;
        inline bool empty() const;

        inline const LineSet& lines() const;

		inline SequenceType sequence() const;
		inline void sequence(const SequenceType seq);

	private:
		SequenceType sequence_;
        LineSet lines_;
	};
	

	template<typename SequenceType, std::size_t NumberOfLines>
	SequenceInfo<SequenceType, NumberOfLines>::SequenceInfo()
        : sequence_()
	{
        lines_.fill();
	}
	
	template<typename SequenceType, std::size_t NumberOfLines>
	SequenceInfo<SequenceType, NumberOfLines>::SequenceInfo(const std::size_t lineId, const SequenceType sequence)
		: sequence_(sequence)
	{
        lines_.insert(lineId);
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

    template<typename SequenceType, std::size_t NumberOfLines>
    bool SequenceInfo<SequenceType, NumberOfLines>::empty() const
    {
        return lines_.empty();
    }

    template<typename SequenceType, std::size_t NumberOfLines>
    const typename SequenceInfo<SequenceType, NumberOfLines>::LineSet& SequenceInfo<SequenceType, NumberOfLines>::lines() const
    {
        return lines_;
    }

    template<typename SequenceType, std::size_t NumberOfLines>
    void SequenceInfo<SequenceType, NumberOfLines>::insert(const std::size_t lineId)
    {
        lines_.insert(lineId);
    }

    template<typename SequenceType, std::size_t NumberOfLines>
    bool SequenceInfo<SequenceType, NumberOfLines>::has(const std::size_t lineId)
    {
        return lines_[lineId];
    }
}}
