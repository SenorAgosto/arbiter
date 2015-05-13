#include <arbiter/Exceptions.hpp>
#include <string>

namespace arbiter {

    ArbiterCacheAdvancerStateEnumOutOfRange::ArbiterCacheAdvancerStateEnumOutOfRange(const std::size_t value)
        : std::out_of_range("ArbiterCacheAdvancerStateEnum is out of range, value = " + std::to_string(value))
    {
    }
}
