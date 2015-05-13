#pragma once 

#include <cstddef>
#include <stdexcept> 

namespace arbiter {

    class ArbiterCacheAdvancerStateEnumOutOfRange : public std::out_of_range
    {
    public:
        ArbiterCacheAdvancerStateEnumOutOfRange(const std::size_t value);
    };
}
