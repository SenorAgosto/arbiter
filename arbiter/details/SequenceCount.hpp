#pragma once 
#include <arbiter/Gap.hpp>
#include <cstddef>

namespace arbiter { namespace details {

	// A structure to hold a count of sequence numbers
	template<typename SequenceType>
	class SequenceCount 
	{
	public:
		SequenceCount(const SequenceType sequence);
		SequenceCount(const SequenceType sequence, const std::size_t count);
		SequenceCount(const SequenceType sequence, const GapTag);
		
		inline std::size_t operator++();		// prefix increment 
		inline std::size_t operator++(int);   	// postfix increment 
		
		inline SequenceType sequence() const;
		inline std::size_t count() const;

	private:
		std::size_t count_;
		SequenceType sequence_;
	};
	
	
	template<typename SequenceType>
	SequenceCount<SequenceType>::SequenceCount(const SequenceType sequence)
		: count_(1)
		, sequence_(sequence)
	{
	}

	template<typename SequenceType>
	SequenceCount<SequenceType>::SequenceCount(const SequenceType sequence, const std::size_t count)
		: count_(count)
		, sequence_(sequence)
	{
	}

	template<typename SequenceType>
	SequenceCount<SequenceType>::SequenceCount(const SequenceType sequence, const GapTag)
		: count_(0)
		, sequence_(sequence)
	{
	}

	template<typename SequenceType>
	std::size_t SequenceCount<SequenceType>::operator++()
	{
		return ++count_;
	}
	
	template<typename SequenceType>
	std::size_t SequenceCount<SequenceType>::operator++(int)
	{
		return count_++;
	}
	
	template<typename SequenceType>
	SequenceType SequenceCount<SequenceType>::sequence() const 
	{
		return sequence_;
	}
	
	template<typename SequenceType>
	std::size_t SequenceCount<SequenceType>::count() const 
	{
		return count_;
	}
}}
