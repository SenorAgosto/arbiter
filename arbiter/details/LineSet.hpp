#pragma once 
#include <bitset>
#include <cstddef>
#include <vector>

namespace arbiter { namespace details {

    // track whether a line ID has appeared before
    // or is missing.
    template<std::size_t NumberOfLines>
    class LineSet
    {
    public:

        bool insert(const std::size_t lineId);  // false if lineId in set already
        bool complete() const;   // true if all lines in set

        bool operator[](const std::size_t index);

        std::size_t count() const;
        std::vector<std::size_t> missing() const;

        void fill();

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
    bool LineSet<NumberOfLines>::complete() const
    {
        return value_.count() == NumberOfLines;
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

    template<std::size_t NumberOfLines>
    std::vector<std::size_t> LineSet<NumberOfLines>::missing() const
    {
        std::vector<std::size_t> missingLines;

        for(std::size_t i = 0, end = value_.size(); i < end; ++i)
        {
            if(!value_[i])
            {
                missingLines.emplace_back(i);
            }
        }

        return missingLines;
    }

    template<std::size_t NumberOfLines>
    void LineSet<NumberOfLines>::fill()
    {
        value_.set();
    }
}}
