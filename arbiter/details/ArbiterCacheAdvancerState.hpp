#pragma once 
#include <arbiter/details/ArbiterCacheAdvancerContext.hpp>

namespace arbiter { namespace details {

    template<class Traits>
    class ArbiterCacheAdvancerState
    {
    public:
        using SequenceType = typename Traits::SequenceType;

        virtual ~ArbiterCacheAdvancerState(){}
        virtual bool advance(ArbiterCacheAdvancerContext<Traits>& context, const std::size_t lineId, const SequenceType sequenceNumber) = 0;
    };
}}
