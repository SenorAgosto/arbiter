#include "./platform/UnitTestSupport.hpp"
#include <arbiter/details/SequenceCount.hpp>

namespace {

	TEST(verifyInstantiationOfSequenceCount)
	{
		arbiter::details::SequenceCount<std::size_t> sequenceCount(0);
		
		CHECK_EQUAL(1U, sequenceCount.count());
		CHECK_EQUAL(0U, sequenceCount.sequence());
	}
	
	TEST(verifyInstantiationWithCount)
	{
		arbiter::details::SequenceCount<std::size_t> sequenceCount(0, 10);

		CHECK_EQUAL(10U, sequenceCount.count());
		CHECK_EQUAL(0U, sequenceCount.sequence());
	}
	
	TEST(verifyInstantiationWithGap)
	{
		arbiter::details::SequenceCount<std::size_t> sequenceCount(0, arbiter::gap);
		
		CHECK_EQUAL(0U, sequenceCount.count());
		CHECK_EQUAL(0U, sequenceCount.sequence());
	}
	
	TEST(verifyPrefixIncrement)
	{
		arbiter::details::SequenceCount<std::size_t> sequenceCount(0);
		
		CHECK_EQUAL(1U, sequenceCount.count());
		auto s = ++sequenceCount;
		
		CHECK_EQUAL(2U, s);
		CHECK_EQUAL(2U, sequenceCount.count());
	}
	
	TEST(verifyPostfixIncrement)
	{
		arbiter::details::SequenceCount<std::size_t> sequenceCount(0);
		
		CHECK_EQUAL(1U, sequenceCount.count());
		auto s = sequenceCount++;
		
		CHECK_EQUAL(1U, s);
		CHECK_EQUAL(2U, sequenceCount.count());
	}
}
