#pragma once 
#include <cstddef>

namespace arbiter { namespace details {

	// A structure to hold a count of sequence numbers
	template<typename SequenceType>
	class SequenceInfo 
	{
	public:
		SequenceInfo();
		SequenceInfo(const SequenceType sequence);
		
		inline std::size_t operator++();		// prefix increment 
		inline std::size_t operator++(int);   	// postfix increment 
		
		inline SequenceType sequence() const;
		inline void sequence(const SequenceType seq);

		inline std::size_t count() const;
		inline void count(const std::size_t value);

		inline void invalidate();
		inline bool invalid() const;

	private:
		std::size_t count_;
		SequenceType sequence_;
	};
	

	template<typename SequenceType>
	SequenceInfo<SequenceType>::SequenceInfo()
		: count_(std::numeric_limits<std::size_t>::max())
		, sequence_()
	{
	}
	
	template<typename SequenceType>
	SequenceInfo<SequenceType>::SequenceInfo(const SequenceType sequence)
		: count_(1)
		, sequence_(sequence)
	{
	}

	template<typename SequenceType>
	std::size_t SequenceInfo<SequenceType>::operator++()
	{
		return ++count_;
	}
	
	template<typename SequenceType>
	std::size_t SequenceInfo<SequenceType>::operator++(int)
	{
		return count_++;
	}
	
	template<typename SequenceType>
	SequenceType SequenceInfo<SequenceType>::sequence() const 
	{
		return sequence_;
	}
	
	template<typename SequenceType>
	void SequenceInfo<SequenceType>::sequence(const SequenceType seq)
	{
		sequence_ = seq;
	}

	template<typename SequenceType>
	std::size_t SequenceInfo<SequenceType>::count() const 
	{
		return count_;
	}

	template<typename SequenceType>
	void SequenceInfo<SequenceType>::count(const std::size_t value) 
	{
		count_ = value;
	}
	
	template<typename SequenceType>
	void SequenceInfo<SequenceType>::invalidate()
	{
		count_ = std::numeric_limits<std::size_t>::max();
	}

	template<typename SequenceType>
	bool SequenceInfo<SequenceType>::invalid() const 
	{
		return count_ == std::numeric_limits<std::size_t>::max();
	}	
}}
