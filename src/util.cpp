#include "util.hpp"
#include <sstream>
#include <algorithm>
#include <vector>
#include <iomanip>
#include "date.h"

namespace brite {

    std::vector<std::string>& _split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            if (!item.empty())
                elems.push_back(item);
        }
        return elems;
    }

    std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        _split(s, delim, elems);
        return elems;
    }
    
    std::string concat(const std::vector<std::string>& v) {
        std::string res;
        for (auto it = v.begin(); it != v.end(); ++it) {
           res += "/" + *it; 
        }
        return res;
    }

    unsigned long dateTimeToTimestamp(const std::string& date) 
    {
        using namespace date;
        using namespace std::chrono;

        auto dd = split(date, 'T');
        if (dd.size() != 2) { // wrong date iso 8601 format. 
            return 0;
        }
        auto ddate = split(dd[0], '-'); 
        if (ddate.size() != 3) { // wrong date iso 8601 format
            return 0;
        }
        auto dtime = split(dd[1], ':'); 
        if (dtime.size() != 3) {
            return 0;
        }
        
        int y = std::stoi(ddate[0]);
        int m = std::stoi(ddate[1]);
        int d = std::stoi(ddate[2]);
        //
        int h = std::stoi(dtime[0]);
        int M = std::stoi(dtime[1]);
        int s = std::stoi(dtime[2]);

        date::sys_seconds sec = sys_days{year{y}/m/d} + hours{h} + minutes{M} + seconds{s};
        return sec.time_since_epoch().count();
    }

    unsigned long timestamp()
    {
        using namespace std::chrono;
        return duration_cast<std::chrono::seconds>(system_clock::now().time_since_epoch()).count();
    }

    std::string timestampToDateTime(const unsigned long& timestamp)
    {
        using namespace std::chrono;
        // time wihout miliseconds.
        auto tp = timestamp ?
            time_point<system_clock>(milliseconds(timestamp)) :
            system_clock::now();

        auto dp = date::floor<date::days>(tp); // create day point
        auto ymd = date::year_month_day{dp};
        auto time = date::make_time(duration_cast<milliseconds>(tp-dp));

        std::stringstream mytime;
        mytime << ymd.year() << "-";
        mytime << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd.month()) << "-"; // cast in int
        mytime << std::setw(2) << std::setfill('0') << ymd.day() << "T";
        mytime << std::setw(2) << std::setfill('0') << time.hours().count() << ":";
        mytime << std::setw(2) << std::setfill('0') << time.minutes().count() << ":";
        mytime << std::setw(2) << std::setfill('0') << time.seconds().count() << ".";
        mytime << std::setw(3) << std::setfill('0') << time.subseconds().count() << "Z";

        return mytime.str();
    }
}  // namespace brite 
