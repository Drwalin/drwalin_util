// Copyright (C) 2024-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_RANDOM_HPP
#define DRWUTIL_RANDOM_HPP

#include <cstdint>

namespace dutl
{
struct mt_engine {
};

struct random_t {
	static uint64_t default_seed();

	random_t();
	random_t(uint64_t seed);
	~random_t();

	void seed(uint64_t seed);

	float randf();
	double randd();
	float randf(float min, float max);
	double randd(double min, double max);

	int64_t randi(int64_t min, int64_t max);
	uint64_t randu(uint64_t min, uint64_t max);
	uint64_t randu();

	float normal(float min = 0, float max = 1, int iterations = 4);

private:
	mt_engine *mt;
};

extern random_t random;
} // namespace dutl

#endif
