#include <cmath>
#include <atomic>
#include <iostream>
#include <memory>
#include <random>
#include "sound.hxx"
#include "generator.hxx"

typedef std::int16_t Value;

struct Sample
{
	Value left;
	Value right;
};

static_assert(sizeof(Sample) == 4, "Sample size must be exactly 4 bytes");

static long const buf_length = 4096;

static SDL_AudioDeviceID dev;
long sample_rate;

std::atomic<double> value_left;
std::atomic<double> value_right;

std::ranlux24 main_rnd;
NoiseGenerator2 *gen_left;
NoiseGenerator2 *gen_right;

static void SDLCALL audioCB(void *userdata, Uint8 *stream, int bytes)
{
	gen_left->power = std::abs(value_left.load());
	gen_right->power = std::abs(value_right.load());
	double pow = std::max(gen_left->power, gen_right->power);
	long samples = bytes / sizeof(Sample);
	if(bytes % sizeof(Sample))
		throw std::logic_error("Invalid sample size");
	Sample *ptr = reinterpret_cast<Sample *>(stream);
	while(samples--)
	{
		ptr->left = static_cast<Value>(32000.0 * gen_left->next());
		ptr->right = static_cast<Value>(32000.0 * gen_right->next());
		++ptr;
	}
}

void initAudio()
{
	SDL_AudioSpec want, have;
	std::cout << "Initializing audio" << std::endl;
	std::memset(&want, 0, sizeof(want));
	want.freq = 48000;
	want.format = AUDIO_S16SYS;
	want.channels = 2;
	want.samples = buf_length;
	want.callback = audioCB;

	dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	if(!dev)
		throw std::runtime_error("Failed to open audio");
	std::cout << "Audio sample rate: " << have.freq << std::endl;
	std::cout << "Buffer size (samples): " << have.samples << std::endl;
	std::cout << "Buffer size (bytes): " << have.size << std::endl;

	sample_rate = have.freq;
	gen_left = new NoiseGenerator2(main_rnd);
	gen_right = new NoiseGenerator2(main_rnd);

	SDL_PauseAudioDevice(dev, 0);
}

void closeAudio()
{
	SDL_CloseAudioDevice(dev);
}

void updateAudio(Float left, Float right, Float time)
{
	value_left.store(left);
	value_right.store(right);
}
