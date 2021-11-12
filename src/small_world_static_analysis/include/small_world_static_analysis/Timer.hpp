#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace small_world::static_analysis {

template<typename Rep, typename Period>
inline std::ostream& operator<<(std::ostream& strm, std::chrono::duration<Rep, Period> dur) {
    std::chrono::hours hrs = std::chrono::duration_cast<std::chrono::hours>(dur);
    dur -= hrs;
    std::chrono::minutes mins = std::chrono::duration_cast<std::chrono::minutes>(dur);
    dur -= mins;
    std::chrono::seconds secs = std::chrono::duration_cast<std::chrono::seconds>(dur);
    dur -= secs;
    std::chrono::milliseconds millis = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
    dur -= millis;
    std::chrono::microseconds micros = std::chrono::duration_cast<std::chrono::microseconds>(dur);
    dur -= micros;
    std::chrono::nanoseconds nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
    dur -= nanos;
    bool pt = false;
    pt |= hrs.count();
    if (pt)
        strm << hrs.count() << "h ";
    pt |= mins.count();
    if (pt)
        strm << mins.count() << "m ";
    pt |= secs.count();
    if (pt)
        strm << secs.count() << "s ";
    pt |= millis.count();
    if (pt)
        strm << millis.count() << "ms ";
    pt |= micros.count();
    if (pt)
        strm << micros.count() << "µs ";
    pt |= nanos.count();
    if (pt)
        strm << nanos.count() << "ns";
    else
        strm << 0;
    return strm;
}

template<typename Clock = std::chrono::high_resolution_clock>
struct Timer {
    using clock = Clock;
    using time_point = clock::time_point;
    using duration = clock::duration;

    clock::time_point start;

    inline Timer() :
        start(clock::now())
    {}

    inline clock::duration elapsed() const {
        return clock::now() - start;
    }

    inline void clear() {
        start = clock::now();
    }

    friend std::ostream& operator<<(std::ostream& strm, const Timer& timer) {
        return strm << timer.elapsed();
    }

    inline std::string str() const {
        std::stringstream strm;
        strm << elapsed();
        return strm.str();
    }
};

}