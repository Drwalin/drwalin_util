// Copyright (C) 2025-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_TIME_HPP
#define DRWUTIL_TIME_HPP

#include <cstdint>

namespace dutl
{
namespace time
{
struct diff {
	int64_t ns = 0;
	inline constexpr diff operator-() const { return {-ns}; }
	
	inline constexpr float sec() const { return ns * (1.0f / 1'000'000'000.0f); }
	inline constexpr float msec() const { return ns * (1.0f / 1'000'000.0f); }
	inline constexpr float usec() const { return ns * (1.0f / 1'000.0f); }
};
using duration = diff;

struct point {
	int64_t ns = 0;
	
	inline constexpr float sec() { return ns * (1.0f / 1'000'000'000.0f); }
	inline constexpr float msec() const { return ns * (1.0f / 1'000'000.0f); }
	inline constexpr float usec() const { return ns * (1.0f / 1'000.0f); }
};

struct timestamp {
	int64_t ns = 0;
};

point now();
void sleep_for(diff dt);

inline constexpr diff abs(diff t) { return t.ns < 0 ? diff{-t.ns} : t; }
inline constexpr diff clamp(diff t, diff min, diff max) { return t.ns < min.ns ? min : t.ns > max.ns ? max : t; }
inline constexpr diff min(diff a, diff b) { return a.ns < b.ns ? a : b; }
inline constexpr diff max(diff a, diff b) { return a.ns > b.ns ? a : b; }

inline constexpr diff nanoseconds(int t)  { return {t}; }
inline constexpr diff microseconds(int t) { return {t * 1000ll}; }
inline constexpr diff milliseconds(int t) { return {t * 1000000ll}; }
inline constexpr diff seconds(int t)      { return {t * 1000000000ll}; }
inline constexpr diff minutes(int t)      { return {seconds(t).ns * 60ll}; }
inline constexpr diff hours(int t)        { return {minutes(t).ns * 60ll}; }

inline constexpr diff nanoseconds(int64_t t)  { return {t}; }
inline constexpr diff microseconds(int64_t t) { return {t * 1000ll}; }
inline constexpr diff milliseconds(int64_t t) { return {t * 1000000ll}; }
inline constexpr diff seconds(int64_t t)      { return {t * 1000000000ll}; }
inline constexpr diff minutes(int64_t t)      { return {seconds(t).ns * 60ll}; }
inline constexpr diff hours(int64_t t)        { return {minutes(t).ns * 60ll}; }

inline constexpr diff nanoseconds(unsigned int t)  { return {t}; }
inline constexpr diff microseconds(unsigned int t) { return {t * 1000ll}; }
inline constexpr diff milliseconds(unsigned int t) { return {t * 1000000ll}; }
inline constexpr diff seconds(unsigned int t)      { return {t * 1000000000ll}; }
inline constexpr diff minutes(unsigned int t)      { return {seconds(t).ns * 60ll}; }
inline constexpr diff hours(unsigned int t)        { return {minutes(t).ns * 60ll}; }

inline constexpr diff nanoseconds(uint64_t t)  { return {(int64_t)t}; }
inline constexpr diff microseconds(uint64_t t) { return {int64_t(t * 1000ll)}; }
inline constexpr diff milliseconds(uint64_t t) { return {int64_t(t * 1000000ll)}; }
inline constexpr diff seconds(uint64_t t)      { return {int64_t(t * 1000000000ll)}; }
inline constexpr diff minutes(uint64_t t)      { return {seconds(t).ns * 60ll}; }
inline constexpr diff hours(uint64_t t)        { return {minutes(t).ns * 60ll}; }

inline constexpr diff microseconds(double t) { return {int64_t(t * 1000.0)}; }
inline constexpr diff milliseconds(double t) { return {int64_t(t * 1000000.0)}; }
inline constexpr diff seconds(double t)      { return {int64_t(t * 1000000000.0)}; }
inline constexpr diff minutes(double t)      { return {int64_t(seconds(t).ns * 60.0)}; }
inline constexpr diff hours(double t)        { return {int64_t(minutes(t).ns * 60.0)}; }

inline constexpr diff microseconds(float t) { return {int64_t(t * 1000.0f)}; }
inline constexpr diff milliseconds(float t) { return {int64_t(t * 1000000.0f)}; }
inline constexpr diff seconds(float t)      { return {int64_t(t * 1000000000.0f)}; }
inline constexpr diff minutes(float t)      { return {int64_t(seconds(t).ns * 60.0f)}; }
inline constexpr diff hours(float t)        { return {int64_t(minutes(t).ns * 60.0f)}; }

point get_temporary_timestamp();
timestamp temporary_timestamp_to_timestamp(point tmp_timestamp);
timestamp get_timestamp();

double delta_sec_between_timestamps(timestamp begin, timestamp end);
double delta_msec_between_timestamps(timestamp begin, timestamp end);
double delta_usec_between_timestamps(timestamp begin, timestamp end);
diff delta_ns_between_timestamps(timestamp begin, timestamp end);
double nanoseconds_to_seconds(diff ns);

double delta_sec_between_timepoints(point begin, point end);
double delta_msec_between_timepoints(point begin, point end);
double delta_usec_between_timepoints(point begin, point end);
diff delta_ns_between_timepoints(point begin, point end);

struct time_string
{
	char str[64];
};

time_string timestamp_to_string(timestamp ts, int subseconds_digits);
time_string get_current_timestamp_string(int subseconds_digits);
timestamp string_to_timestamp(const char *str);
void ymd_from_timestamp(timestamp ts, int &day, int &month, int &year);
timestamp timestamp_from_ymd(int day, int month, int year);

void sleep(diff timediff);

inline bool operator<(timestamp l, timestamp r) { return l.ns < r.ns; }
inline bool operator<=(timestamp l, timestamp r) { return l.ns <= r.ns; }
inline bool operator>(timestamp l, timestamp r) { return l.ns > r.ns; }
inline bool operator>=(timestamp l, timestamp r) { return l.ns >= r.ns; }
inline bool operator==(timestamp l, timestamp r) { return l.ns == r.ns; }
inline bool operator!=(timestamp l, timestamp r) { return l.ns != r.ns; }

inline timestamp operator+(timestamp p, diff d) { return {p.ns + d.ns}; }
inline timestamp operator-(timestamp p, diff d) { return {p.ns - d.ns}; }

inline timestamp &operator+=(timestamp &p, diff d)
{
	p.ns += d.ns;
	return p;
}
inline timestamp &operator-=(timestamp &p, diff d)
{
	p.ns -= d.ns;
	return p;
}

inline diff operator-(timestamp a, timestamp b) { return {a.ns - b.ns}; }
} // namespace time
} // namespace dutl

inline constexpr bool operator< (dutl::time::diff l, dutl::time::diff r) { return l.ns < r.ns; }
inline constexpr bool operator<=(dutl::time::diff l, dutl::time::diff r) { return l.ns <= r.ns; }
inline constexpr bool operator> (dutl::time::diff l, dutl::time::diff r) { return l.ns > r.ns; }
inline constexpr bool operator>=(dutl::time::diff l, dutl::time::diff r) { return l.ns >= r.ns; }
inline constexpr bool operator==(dutl::time::diff l, dutl::time::diff r) { return l.ns == r.ns; }
inline constexpr bool operator!=(dutl::time::diff l, dutl::time::diff r) { return l.ns != r.ns; }

inline constexpr bool operator< (dutl::time::point l, dutl::time::point r) { return l.ns < r.ns; }
inline constexpr bool operator<=(dutl::time::point l, dutl::time::point r) { return l.ns <= r.ns; }
inline constexpr bool operator> (dutl::time::point l, dutl::time::point r) { return l.ns > r.ns; }
inline constexpr bool operator>=(dutl::time::point l, dutl::time::point r) { return l.ns >= r.ns; }
inline constexpr bool operator==(dutl::time::point l, dutl::time::point r) { return l.ns == r.ns; }
inline constexpr bool operator!=(dutl::time::point l, dutl::time::point r) { return l.ns != r.ns; }

inline constexpr dutl::time::point operator+(dutl::time::point p, dutl::time::diff d) { return {p.ns + d.ns}; }
inline constexpr dutl::time::point operator-(dutl::time::point p, dutl::time::diff d) { return {p.ns - d.ns}; }

inline constexpr dutl::time::point &operator+=(dutl::time::point &p, dutl::time::diff d) { p.ns += d.ns; return p; }
inline constexpr dutl::time::point &operator-=(dutl::time::point &p, dutl::time::diff d) { p.ns -= d.ns; return p; }

inline constexpr dutl::time::diff operator-(dutl::time::point a, dutl::time::point b) { return {a.ns - b.ns}; }
inline constexpr dutl::time::diff operator-(dutl::time::diff a, dutl::time::diff b)   { return {a.ns - b.ns}; }
inline constexpr dutl::time::diff operator+(dutl::time::diff a, dutl::time::diff b)   { return {a.ns + b.ns}; }

inline constexpr dutl::time::diff &operator+=(dutl::time::diff &a, dutl::time::diff b) { a.ns += b.ns; return a; }
inline constexpr dutl::time::diff &operator-=(dutl::time::diff &a, dutl::time::diff b) { a.ns -= b.ns; return a; }

inline constexpr dutl::time::diff operator*(dutl::time::diff a, int v)     { return {a.ns * v}; }
inline constexpr dutl::time::diff operator/(dutl::time::diff a, int v)     { return {a.ns / v}; }
inline constexpr dutl::time::diff operator*(dutl::time::diff a, int64_t v) { return {a.ns * v}; }
inline constexpr dutl::time::diff operator/(dutl::time::diff a, int64_t v) { return {a.ns / v}; }
inline constexpr dutl::time::diff operator*(dutl::time::diff a, float v)   { return {int64_t(a.ns * v)}; }
inline constexpr dutl::time::diff operator/(dutl::time::diff a, float v)   { return {int64_t(a.ns / v)}; }
inline constexpr dutl::time::diff operator*(dutl::time::diff a, double v)  { return {int64_t(a.ns * v)}; }
inline constexpr dutl::time::diff operator/(dutl::time::diff a, double v)  { return {int64_t(a.ns / v)}; }

inline constexpr dutl::time::diff &operator*=(dutl::time::diff &a, int v)     { a.ns *= v; return a; }
inline constexpr dutl::time::diff &operator/=(dutl::time::diff &a, int v)     { a.ns /= v; return a; }
inline constexpr dutl::time::diff &operator*=(dutl::time::diff &a, int64_t v) { a.ns *= v; return a; }
inline constexpr dutl::time::diff &operator/=(dutl::time::diff &a, int64_t v) { a.ns /= v; return a; }
inline constexpr dutl::time::diff &operator*=(dutl::time::diff &a, float v)   { a.ns *= v; return a; }
inline constexpr dutl::time::diff &operator/=(dutl::time::diff &a, float v)   { a.ns /= v; return a; }
inline constexpr dutl::time::diff &operator*=(dutl::time::diff &a, double v)  { a.ns *= v; return a; }
inline constexpr dutl::time::diff &operator/=(dutl::time::diff &a, double v)  { a.ns /= v; return a; }

#endif
