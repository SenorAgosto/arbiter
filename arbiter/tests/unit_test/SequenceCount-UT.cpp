#include "./platform/UnitTestSupport.hpp"
#include <arbiter/details/SequenceCount.hpp>

namespace {

	TEST(verifyInstantiationOfSequenceCount)
	{
		arbiter::details::SequenceCount<std::size_t> sequenceCount(0);
		
		CHECK_EQUAL(1U, sequenceCount.count());
		CHECK_EQUAL(0U, sequenceCount.sequence());
	}
	
	TEST(verifyInstantiationWithDefaultConstructor)
	{
		arbiter::details::SequenceCount<std::size_t> sequenceCount;
		CHECK(sequenceCount.invalid());
	}
	
	TEST(verifyInvalidate)
	{
		arbiter::details::SequenceCount<std::size_t> sequenceCount(0);
		
		CHECK(!sequenceCount.invalid());
		sequenceCount.invalidate();
		CHECK(sequenceCount.invalid());
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
