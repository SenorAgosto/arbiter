#pragma once 
#include <bitset>

namespace arbiter { namespace details {

    // track whether a line ID has appeared before
    // or is missing.
    template<std::size_t NumberOfLines>
    class LineSet
    {
    public:
        // returns false if lineId is already in the set.
        bool insert(const std::size_t lineId);
        bool missing() const;   // returns true if there are lines missing.

        bool operator[](const std::size_t index);

        std::size_t count() const;

    private:
        std::bitset<NumberOfLines> value_;
    };


    template<std::size_t NumberOfLines>
    bool LineSet<NumberOfLines>::insert(const std::size_t lineId)
    {
        bool returnValue = value_.test(lineId) == false;
        value_.set(lineId);

        return returnValue;
    }

    template<std::size_t NumberOfLines>
    bool LineSet<NumberOfLines>::missing() const
    {
        return value_.count() != NumberOfLines;
    }

    template<std::size_t NumberOfLines>
    bool LineSet<NumberOfLines>::operator[](const std::size_t index)
    {
        return value_[index];
    }

    template<std::size_t NumberOfLines>
    std::size_t LineSet<NumberOfLines>::count() const
    {
        return value_.count();
    }
}}
