#include "generator.hxx"

extern long sample_rate;

std::normal_distribution<> NoiseGenerator2::ctlrnd(10.0, 0.1);

SineWaveGenerator::SineWaveGenerator(double freq)
{
	setfreq(freq);
}

double SineWaveGenerator::next()
{
	phase += dphi;
	if(phase > 2 * M_PI)
		phase -= 2 * M_PI;
	return std::sin(phase);
}

void SineWaveGenerator::skip(long samples)
{
	phase = std::fmod(phase + samples * dphi, 2 * M_PI);
}

void SineWaveGenerator::skip(double time)
{
	long samples = static_cast<long>(sample_rate * time);
	skip(samples);
	phase += time - (static_cast<double>(samples) / sample_rate);
}

void SineWaveGenerator::setfreq(double freq)
{
	dphi = 2 * M_PI * freq / sample_rate;
}

NoiseGenerator1::NoiseGenerator1(long seed) :
	rnd(seed)
{
	setparam();
}

double NoiseGenerator1::next()
{
	time -= 1.0 / sample_rate;
	if(time <= 0)
	{
		value += vdistr(rnd);
		time += tdistr(rnd);
		if(std::abs(value) > 1.0)
			value = std::copysign(0.9, value);
	}
	return value;
}

void NoiseGenerator1::setparam(double a, double b)
{
	vdistr = std::normal_distribution<>(0.0, vdistr_baseval * std::sqrt(a * b));
	tdistr = std::lognormal_distribution<>(tdistr_baseval - a, 1.0);
}

double NoiseGenerator2::next()
{
	base.setparam(2.0 * power);
	mix.setparam(1.5 + mixctl.next(), 0.01 * std::pow(power, 0.25));
	return 0.5 * (base.next() + mix.next());
}
