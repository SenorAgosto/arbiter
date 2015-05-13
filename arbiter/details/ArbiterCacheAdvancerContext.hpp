#pragma once 
#include <arbiter/details/ArbiterCache.hpp>

namespace arbiter { namespace details {

    // A context object to hold cache & error policy...
    template<class Traits>
    struct ArbiterCacheAdvancerContext
    {
        using ErrorReportingPolicy = typename Traits::ErrorReportingPolicy;

        ArbiterCacheAdvancerContext(ArbiterCache<Traits>& cache, ErrorReportingPolicy& errorPolicy)
            : cache_(cache)
            , errorPolicy_(errorPolicy)
        {
        }

        ArbiterCache<Traits>& cache_;
        ErrorReportingPolicy& errorPolicy_;
    };
}}
