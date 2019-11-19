#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace brite {
    
    std::vector<std::string>& _split(const std::string &s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, char delim = '/');

    std::string concat(const std::vector<std::string>& v);

    unsigned long dateTimeToTimestamp(const std::string& date);
    unsigned long timestamp();
    std::string timestampToDateTime(const unsigned long& timestamp = 0);

    /**
     *  Provide time literals for easy use of time values.
     */
    // returns in chrono milliseconds
    constexpr std::chrono::milliseconds operator "" _ms(unsigned long long ms)
    {
        return std::chrono::milliseconds(ms);
    }

    // returns in seconds
    constexpr long double operator "" _ms(long double ms)
    {
        return std::chrono::duration<long double>(ms/1000).count();
    }

    // returns in chrono milliseconds
    constexpr std::chrono::milliseconds operator "" _s(unsigned long long s)
    {
        return std::chrono::seconds(s);
    }

    // returns in seconds
    constexpr long double operator "" _s(long double s)
    {
        return std::chrono::duration<long double>(s).count();
    }

    constexpr std::chrono::milliseconds operator "" _min(unsigned long long min)
    {
        return std::chrono::minutes(min);
    }

    // returns in seconds
    constexpr long double operator "" _min(long double min)
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<long double, std::ratio<60, 1>>(min)).count();
    }

    // returns in chrono milliseconds
    constexpr std::chrono::milliseconds operator "" _h(unsigned long long h)
    {
        return std::chrono::hours(h);
    }

    // returns in seconds.
    constexpr long double operator "" _h(long double h)
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<long double, std::ratio<3600, 1>>(h)).count();
    }

} // namespace qplus
