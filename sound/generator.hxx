#pragma once
#include <random>

struct SineWaveGenerator
{
	double phase = 0.0;
	double dphi;

	SineWaveGenerator(double freq = 440.0);
	double next();
	void skip(long samples);
	void skip(double time);
	void setfreq(double freq);
};

struct NoiseGenerator1
{
	std::ranlux24 rnd;
	std::normal_distribution<> vdistr;
	std::lognormal_distribution<> tdistr;
	static constexpr double const vdistr_baseval = 0.05;
	static constexpr double const tdistr_baseval = -std::log(880.0);

	double time = 0.0;
	double value = 0.0;

	NoiseGenerator1(long seed = 0);
	double next();
	void setparam(double a = 1.0, double b = 1.0);
};

struct NoiseGenerator2
{
	static std::normal_distribution<> ctlrnd;
	NoiseGenerator1 base;
	NoiseGenerator1 mix;
	SineWaveGenerator mixctl;
	double power;

	template<typename _RandomNumberEngine>
	NoiseGenerator2(_RandomNumberEngine rand) :
		base(rand()),
		mix(rand()),
		mixctl(ctlrnd(rand)),
		power(1.0)
	{
		mixctl.skip(5.0);
	}

	double next();
};
