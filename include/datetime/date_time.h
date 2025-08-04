#pragma once
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std::chrono;

namespace datetime {

struct time_span {
    duration<double> _duration;

    static time_span from_seconds(double sec) { return time_span {duration<double>(sec)}; }

    static time_span from_minutes(double min) { return time_span {duration<double>(min * 60)}; }

    static time_span from_hours(double h) { return time_span {duration<double>(h * 3600)}; }

    static time_span from_days(double d) { return time_span {duration<double>(d * 86400)}; }

    double total_seconds() const { return _duration.count(); }

    double total_minutes() const { return _duration.count() / 60.0; }

    double total_hours() const { return _duration.count() / 3600.0; }

    double total_days() const { return _duration.count() / 86400.0; }

    time_span operator+(const time_span& other) const {
        return time_span {_duration + other._duration};
    }

    time_span operator-(const time_span& other) const {
        return time_span {_duration - other._duration};
    }

    time_span operator*(double factor) const { return time_span {_duration * factor}; }

    time_span operator/(double divisor) const { return time_span {_duration / divisor}; }

    bool operator<(const time_span& other) const { return _duration < other._duration; }

    bool operator>(const time_span& other) const { return _duration > other._duration; }

    bool operator==(const time_span& other) const { return _duration == other._duration; }

    /*constexpr time_span operator""_ms(double d) {

    }*/
};

class date_time {
    system_clock::time_point _tp;

    using days_t    = duration<double, std::ratio<86400>>;
    using hours_t   = duration<double, std::ratio<3600>>;
    using minutes_t = duration<double, std::ratio<60>>;
    using seconds_t = duration<double>;

    std::time_t get_time() const { return system_clock::to_time_t(_tp); }

    static std::tm get_tm(const std::time_t& tt) { return *std::localtime(&tt); }

    std::tm get_tm() const { return get_tm(get_time()); }

    template<typename TClock, typename TDuration>
    system_clock::time_point get_tp(time_point<TClock, TDuration>& tp) const {
        static_assert(std::is_same_v<TClock, system_clock>, "Non matching clock types.");
        return time_point_cast<system_clock::duration>(tp);
    }

  public:
    date_time()
        : _tp() {};

    date_time(system_clock::time_point tp)
        : _tp(tp) {}

    template<typename Duration>
    date_time(
        time_point<
            system_clock,
            Duration> tp
    )
        : _tp() {
        _tp = time_point_cast<system_clock::duration>(tp);
    }

    date_time(std::time_t time)
        : _tp(system_clock::from_time_t(time)) {}

    date_time(
        int year,
        int month,
        int day,
        int hour   = 0,
        int minute = 0,
        int second = 0
    )
        : _tp() {
        std::tm t      = {};
        t.tm_year      = year - 1900;
        t.tm_mon       = month - 1;
        t.tm_mday      = day;
        t.tm_hour      = hour;
        t.tm_min       = minute;
        t.tm_sec       = second;
        std::time_t tt = std::mktime(&t);
        _tp            = system_clock::from_time_t(tt);
    }

    // --- Static factories ---
    static date_time now() { return date_time {system_clock::now()}; }

    std::string to_string(const char* format = "%Y-%m-%d %H:%M:%S") const {
        std::time_t tt   = system_clock::to_time_t(_tp);
        std::tm local_tm = *std::localtime(&tt);
        std::ostringstream oss;
        oss << std::put_time(&local_tm, format);
        return oss.str();
    }

    date_time add_time(const std::time_t& time) const { return date_time {get_time() + time}; }

    date_time add(const time_span& span) const { return date_time {_tp + cast(span)}; }

    date_time add_years(int years) const {
        auto tm = get_tm();
        tm.tm_year += years;

        return date_time {std::mktime(&tm)};
    }

    date_time add_months(int months) const {
        auto tm = get_tm();
        tm.tm_mon += months;

        return date_time {std::mktime(&tm)};
    }

    date_time add_days(double days) const { 
        return date_time {_tp + days_t(days)}; 
    }

    date_time add_hours(double hrs) const { return date_time {_tp + hours_t(hrs)}; }

    date_time add_minutes(int mins) const { return date_time {_tp + minutes(mins)}; }

    date_time add_seconds(int sec) const { return date_time {_tp + seconds(sec)}; }

    time_span operator-(const date_time& other) const {
        return time_span {duration<double>(_tp - other._tp)};
    }

    date_time operator+(const time_span& span) const {
        auto dur = cast(span);
        return date_time {_tp + dur};
    }

    date_time operator-(const time_span& span) const {
        auto dur = cast(span);
        return date_time {_tp - dur};
    }

    bool operator<(const date_time& other) const { return _tp < other._tp; }

    bool operator>(const date_time& other) const { return _tp > other._tp; }

    bool operator==(const date_time& other) const { return _tp == other._tp; }

    std::ostream& operator<<(const std::ostream& other) {}

  private:
    template <typename T>
    static system_clock::duration cast(const T& value) {

        if constexpr (std::is_arithmetic_v<T>) {
            return duration_cast<system_clock::duration>(duration<double>(value));
        } else if constexpr (std::is_base_of_v<time_span, T>) {
            return duration_cast<system_clock::duration>(value._duration);
        } else if constexpr (std::is_base_of_v<duration<typename T::rep, typename T::period>, T>) {
            return duration_cast<system_clock::duration>(value);
        } else {
            static_assert(sizeof(T) == 0, "Unsupported type for casting");
        }
    }
};

} // namespace datetime