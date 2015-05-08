#include "./platform/UnitTestSupport.hpp"
#include <arbiter/details/SequenceInfo.hpp>

namespace {

	TEST(verifyInstantiationOfSequenceInfo)
	{
		arbiter::details::SequenceInfo<std::size_t, 1> sequenceInfo(0);
		
		CHECK(!sequenceInfo.complete());
        CHECK_EQUAL(0U, sequenceInfo.sequence());
	}

    TEST(verifyInstantiationOfSequenceInfoWithDefaultConstructor)
    {
        arbiter::details::SequenceInfo<std::size_t, 1> sequenceInfo;

        CHECK(sequenceInfo.complete());
        CHECK_EQUAL(0U, sequenceInfo.sequence());
    }
}
