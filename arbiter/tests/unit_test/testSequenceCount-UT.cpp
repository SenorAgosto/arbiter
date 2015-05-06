#include "./platform/UnitTestSupport.hpp"
#include <arbiter/details/SequenceInfo.hpp>

namespace {

	TEST(verifyInstantiationOfSequenceInfo)
	{
		arbiter::details::SequenceInfo<std::size_t> sequenceInfo(0);
		
		CHECK_EQUAL(1U, sequenceInfo.count());
		CHECK_EQUAL(0U, sequenceInfo.sequence());
	}
	
	TEST(verifyInstantiationWithDefaultConstructor)
	{
		arbiter::details::SequenceInfo<std::size_t> sequenceInfo;
		CHECK(sequenceInfo.invalid());
	}
	
	TEST(verifyInvalidate)
	{
		arbiter::details::SequenceInfo<std::size_t> sequenceInfo(0);
		
		CHECK(!sequenceInfo.invalid());
		sequenceInfo.invalidate();
		CHECK(sequenceInfo.invalid());
	}

	TEST(verifyPrefixIncrement)
	{
		arbiter::details::SequenceInfo<std::size_t> sequenceInfo(0);
		
		CHECK_EQUAL(1U, sequenceInfo.count());
		auto s = ++sequenceInfo;
		
		CHECK_EQUAL(2U, s);
		CHECK_EQUAL(2U, sequenceInfo.count());
	}
	
	TEST(verifyPostfixIncrement)
	{
		arbiter::details::SequenceInfo<std::size_t> sequenceInfo(0);
		
		CHECK_EQUAL(1U, sequenceInfo.count());
		auto s = sequenceInfo++;
		
		CHECK_EQUAL(1U, s);
		CHECK_EQUAL(2U, sequenceInfo.count());
	}

	TEST(verifySetters)
	{
		arbiter::details::SequenceInfo<std::size_t> sequenceInfo;
		CHECK(sequenceInfo.invalid());
		
		sequenceInfo.count(100);
		sequenceInfo.sequence(15);

		CHECK(!sequenceInfo.invalid());
		CHECK_EQUAL(100U, sequenceInfo.count());
		CHECK_EQUAL(15U, sequenceInfo.sequence());
	}
}
