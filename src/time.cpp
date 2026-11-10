// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifdef tm_gmtoff
#include <ctime>
#endif

#include <chrono>
#include <thread>

#include "../include/drwalin_util/time.hpp"

namespace dutl
{
namespace time
{
point now()
{
	auto now = std::chrono::steady_clock::now().time_since_epoch();
	return {std::chrono::nanoseconds(now).count()};
}

void sleep_for(diff dt)
{
	std::this_thread::sleep_for(std::chrono::nanoseconds(dt.ns));
}

// using UtcClock = std::chrono::utc_clock;
using UtcClock = std::chrono::system_clock;
struct DayTimeBegin {
	DayTimeBegin()
	{
		date = UtcClock::now();
		time_since_epoch = now();
		nanoseconds_since_epoch =
			std::chrono::duration_cast<std::chrono::nanoseconds, int64_t>(
				date.time_since_epoch())
				.count() -
			time_since_epoch.ns;

#ifdef tm_gmtoff
		time_t secs;
		secs = std::time(nullptr);
		struct tm local = *localtime(&secs);
		hours_offset = -local.tm_gmtoff / 3600;
#else
		hours_offset = 0;
#endif
		hours_offset_nanoseconds =
			hours_offset * 3600ll * 1000ll * 1000ll * 1000ll;
	}
	UtcClock::time_point date;
	point time_since_epoch;
	int64_t nanoseconds_since_epoch;
	int64_t hours_offset;
	int64_t hours_offset_nanoseconds;
};

static const DayTimeBegin &glob()
{
	const static DayTimeBegin v;
	return v;
}

timestamp get_timestamp()
{
	return temporary_timestamp_to_timestamp(get_temporary_timestamp());
}

point get_temporary_timestamp() { return now(); }

timestamp temporary_timestamp_to_timestamp(point tmptimestamp)
{
	return {glob().nanoseconds_since_epoch + tmptimestamp.ns};
}

diff delta_ns_between_timestamps(timestamp begin, timestamp end)
{
	return {end.ns - begin.ns};
}

double delta_sec_between_timestamps(timestamp begin, timestamp end)
{
	return nanoseconds_to_seconds(delta_ns_between_timestamps(begin, end));
}

double deltamsecbetweentimestamps(timestamp begin, timestamp end)
{
	return delta_ns_between_timestamps(begin, end).ns / (1000.0 * 1000.0);
}

double deltausecbetweentimestamps(timestamp begin, timestamp end)
{
	return delta_ns_between_timestamps(begin, end).ns / 1000.0;
}

double nanoseconds_to_seconds(diff ns)
{
	return ns.ns / (1000.0 * 1000.0 * 1000.0);
}

diff delta_ns_between_timepoints(point begin, point end)
{
	return {end.ns - begin.ns};
}

double delta_sec_between_timepoints(point begin, point end)
{
	return nanoseconds_to_seconds(delta_ns_between_timepoints(begin, end));
}

double deltamsecbetweentimepoints(point begin, point end)
{
	return delta_ns_between_timepoints(begin, end).ns / (1000.0 * 1000.0);
}

double deltausecbetweentimepoints(point begin, point end)
{
	return delta_ns_between_timepoints(begin, end).ns / 1000.0;
}

int is_leap_year(int year)
{
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int leap_years_before(int year)
{
	return (year / 4) - (year / 100) + (year / 400);
}

struct ymd {
	int year;
	int month;
	int day;
};

void ymd_from_timestamp(timestamp ts, int &day, int &month, int &year)
{
	int days = ts.ns / (24ll * 3600ll * 1000ll * 1000ll * 1000ll);

	year = 1970 + (days - 370) / 366;
	int leap_years = leap_years_before(year - 1) - leap_years_before(1970);

	days -= (year - 1970) * 365 + leap_years;
	bool is_leap = false;
	while (true) {
		is_leap = is_leap_year(year);
		int max = 365;
		if (is_leap) {
			max = 366;
		}
		if (days >= max) {
			days -= max;
			year++;
		} else {
			break;
		}
	}
	int feb = is_leap ? 29 : 28;
	const int days_per_month[12] = {31, feb, 31, 30, 31, 30,
									31, 31,	 30, 31, 30, 31};

	month = 0;
	for (; days >= days_per_month[month];) {
		days -= days_per_month[month];
		month++;
	}
	month++;
	day = days + 1;
}

ymd ymd_from_timestamp(timestamp ts)
{
	ymd ymd;
	ymd_from_timestamp(ts, ymd.day, ymd.month, ymd.year);
	return ymd;
}

timestamp timestamp_from_ymd(int day, int month, int year)
{
	int64_t days = 0;
	int leap_years = leap_years_before(year - 1) - leap_years_before(1970);

	int feb = is_leap_year(year) ? 29 : 28;
	const int days_per_month[12] = {31, feb, 31, 30, 31, 30,
									31, 31,	 30, 31, 30, 31};

	days += (year - 1970) * 365 + day - 1 + leap_years;

	month--;
	for (int i = 0; i < month; ++i) {
		days += days_per_month[i];
	}

	return {days * 24ll * 3600ll * 1000ll * 1000ll * 1000ll};
}
timestamp timestamp_from_ymd(ymd ymd)
{
	return timestamp_from_ymd(ymd.day, ymd.month, ymd.year);
}

time_string timestamp_to_string(timestamp ts, int subsecond_digits)
{
	ts.ns -= glob().hours_offset_nanoseconds;

	const std::chrono::nanoseconds nanoseconds(ts.ns);
	const std::chrono::seconds seconds =
		std::chrono::duration_cast<std::chrono::seconds, int64_t>(nanoseconds);
	const std::chrono::nanoseconds subseconds = nanoseconds - seconds;

	const int64_t ns = subseconds.count();

	if (subsecond_digits > 9) {
		subsecond_digits = 9;
	}
	if (subsecond_digits < 0) {
		subsecond_digits = 0;
	}

	int subseconds_len = subsecond_digits + ((subsecond_digits + 2) / 3);

	ymd ymd = ymd_from_timestamp(ts);
	int h = (seconds.count() / 3600) % 24, m = (seconds.count() / 60) % 60,
		s = seconds.count() % 60;

	time_string str;
	int size = 0;

	int more = sprintf(str.str + size, "%4.4i-%2.2i-%2.2i+%2.2i:%2.2i:%2.2i",
					   ymd.year, ymd.month, ymd.day, h, m, s);
	size += more;

	char *subseconds_str = str.str + size;
	sprintf(subseconds_str, ".%3.3li.%3.3li.%3.3li", ns / 1000000,
			(ns / 1000) % 1000, ns % 1000);
	subseconds_str[subseconds_len] = 0;
	size += subseconds_len;

	char *tmp = str.str + size;
	more = sprintf(tmp, "U%i", -(int)glob().hours_offset);
	size += more;
	return str;
}

time_string get_current_timestamp_string(int subsecond_digits)
{
	return timestamp_to_string(get_timestamp(), subsecond_digits);
}

timestamp string_to_timestamp(const char *str)
{
	int years = 0, months = 0, days = 0, hours = 0, minutes = 0, seconds = 0;
	sscanf(str, "%d-%d-%d+%d:%d:%d", &years, &months, &days, &hours, &minutes,
		   &seconds);
	std::string subseconds_str = str + 20;
	auto utc_pos = subseconds_str.find("U");
	int utc_offset = 0;
	if (utc_pos != std::string::npos) {
		std::string utc_str = subseconds_str.c_str() + utc_pos;
		subseconds_str.resize(utc_pos);
		sscanf(utc_str.c_str(), "U%d", &utc_offset);
		utc_offset = -utc_offset;
	}
	if (subseconds_str.size() > 7)
		subseconds_str.erase(7, 1);
	if (subseconds_str.size() > 3)
		subseconds_str.erase(3, 1);
	subseconds_str.resize(9, '0');
	int64_t subseconds = atoi(subseconds_str.c_str());

	int64_t t = timestamp_from_ymd({years, months, days}).ns /
					(1000ll * 1000ll * 1000ll) +
				(hours * 60 + minutes) * 60 + seconds;

	int64_t tp_seconds =
		std::chrono::duration_cast<std::chrono::seconds, int64_t>(
			std::chrono::system_clock::from_time_t(t).time_since_epoch())
			.count();

	tp_seconds += utc_offset * 3600ll;
	int64_t ns = ((int64_t)tp_seconds) * 1000000000ll + subseconds;
	return {ns};
}

void sleep(diff dt) { sleep_for(dt); }
} // namespace time
} // namespace dutl
