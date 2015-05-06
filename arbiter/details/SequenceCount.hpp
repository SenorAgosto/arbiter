#pragma once 
#include <cstddef>

namespace arbiter { namespace details {

	// A structure to hold a count of sequence numbers
	template<typename SequenceType>
	class SequenceCount 
	{
	public:
		SequenceCount();
		SequenceCount(const SequenceType sequence);
		
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
	SequenceCount<SequenceType>::SequenceCount()
		: count_(std::numeric_limits<std::size_t>::max())
		, sequence_()
	{
	}
	
	template<typename SequenceType>
	SequenceCount<SequenceType>::SequenceCount(const SequenceType sequence)
		: count_(1)
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
	void SequenceCount<SequenceType>::sequence(const SequenceType seq)
	{
		sequence_ = seq;
	}

	template<typename SequenceType>
	std::size_t SequenceCount<SequenceType>::count() const 
	{
		return count_;
	}

	template<typename SequenceType>
	void SequenceCount<SequenceType>::count(const std::size_t value) 
	{
		count_ = value;
	}
	
	template<typename SequenceType>
	void SequenceCount<SequenceType>::invalidate()
	{
		count_ = std::numeric_limits<std::size_t>::max();
	}

	template<typename SequenceType>
	bool SequenceCount<SequenceType>::invalid() const 
	{
		return count_ == std::numeric_limits<std::size_t>::max();
	}	
}}
