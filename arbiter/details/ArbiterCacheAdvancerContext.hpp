#pragma once 
#include <arbiter/details/ArbiterCache.hpp>

namespace arbiter { namespace details {

    // A context object to hold cache & error policy...
    template<class Traits>
    struct ArbiterCacheAdvancerContext
    {
        using ErrorReportingPolicy = typename Traits::ErrorReportingPolicy;

        ArbiterCacheAdvancerContext(ArbiterCache<Traits>& cacheIn, ErrorReportingPolicy& errorPolicyIn, bool& isFirstCallIn)
            : cache(cacheIn)
            , errorPolicy(errorPolicyIn)
            , isFirstCall(isFirstCallIn)
        {
        }

        ArbiterCache<Traits>& cache;
        ErrorReportingPolicy& errorPolicy;
        bool& isFirstCall;
    };
}}
