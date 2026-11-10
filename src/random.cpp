// Copyright (C) 2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cassert>

#include <random>
#include <mutex>

#include "../include/drwalin_util/random.hpp"

namespace dutl
{
std::mt19937_64 *&_mt(mt_engine *&eng)
{
	return *(std::mt19937_64**)&eng;
}

uint64_t random_t::default_seed()
{
	static std::mutex mutex;
	std::lock_guard lock(mutex);
	static std::random_device rd;
	return rd();
}

random_t::random_t() : random_t(default_seed())
{
}

random_t::random_t(uint64_t seed)
{
	_mt(mt) = new std::mt19937_64(default_seed());
}

random_t::~random_t()
{
	delete _mt(mt);
	_mt(mt) = nullptr;
}

void random_t::seed(uint64_t seed)
{
	_mt(mt)->seed(seed);
}

float random_t::randf()
{
	return (randu() & 0xFFFFFF) / (float)(0xFFFFFF + 1);
}

double random_t::randd()
{
	return (randu() & 0xFFFFFFFFFFFFFFlu) / (double)(0xFFFFFFFFFFFFFFlu + 1lu);
}

float random_t::randf(float min, float max)
{
	return randf() * (max-min) + min;
}

double random_t::randd(double min, double max)
{
	return randf() * (max-min) + min;
}

int64_t random_t::randi(int64_t min, int64_t max)
{
	std::uniform_int_distribution<int64_t> d{min, max};
	return d((*_mt(mt)));
}

uint64_t random_t::randu(uint64_t min, uint64_t max)
{
	std::uniform_int_distribution<uint64_t> d{min, max};
	return d((*_mt(mt)));
}

uint64_t random_t::randu()
{
	return (*_mt(mt))();
}

float random_t::normal(float min, float max, int iterations)
{
	float sum = 0;
	for (int i=0; i<iterations; ++i) {
		sum += randf();
	}
	return (sum / (float)iterations) * (max-min) + min;
}

random_t random;
} // namespace dutl
