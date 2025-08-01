#pragma once
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <cmath>

namespace datetime {

struct time_span {
    std::chrono::duration<double> _duration;

    static time_span from_seconds(double sec) {
        return time_span {std::chrono::duration<double>(sec)};
    }

    static time_span from_minutes(double min) {
        return time_span {std::chrono::duration<double>(min * 60)};
    }

    static time_span from_hours(double h) {
        return time_span {std::chrono::duration<double>(h * 3600)};
    }

    static time_span from_days(double d) {
        return time_span {std::chrono::duration<double>(d * 86400)};
    }

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
};

struct date_time {
    std::chrono::system_clock::time_point _tp;

    // --- Static factories ---
    static date_time now() { return date_time {std::chrono::system_clock::now()}; }

    static date_time from_ymd(
        int year,
        int month,
        int day,
        int hour   = 0,
        int minute = 0,
        int second = 0
    ) {
        std::tm t      = {};
        t.tm_year      = year - 1900;
        t.tm_mon       = month - 1;
        t.tm_mday      = day;
        t.tm_hour      = hour;
        t.tm_min       = minute;
        t.tm_sec       = second;
        std::time_t tt = std::mktime(&t);
        return date_time {std::chrono::system_clock::from_time_t(tt)};
    }

    std::string to_string(const char* format = "%Y-%m-%d %H:%M:%S") const {
        std::time_t tt   = std::chrono::system_clock::to_time_t(_tp);
        std::tm local_tm = *std::localtime(&tt);
        std::ostringstream oss;
        oss << std::put_time(&local_tm, format);
        return oss.str();
    }

    date_time add(const time_span& span) const { return date_time {_tp + span._duration}; }

    date_time add(double seconds) const {
        return date_time {_tp + std::chrono::duration<double>(seconds)};
    }

    time_span operator-(const date_time& other) const {
        return time_span {std::chrono::duration<double>(_tp - other._tp)};
    }

    date_time operator+(const time_span& span) const { return date_time {_tp + span._duration}; }

    date_time operator-(const time_span& span) const { return date_time {_tp - span._duration}; }

    bool operator<(const date_time& other) const { return _tp < other._tp; }

    bool operator>(const date_time& other) const { return _tp > other._tp; }

    bool operator==(const date_time& other) const { return _tp == other._tp; }
};

}