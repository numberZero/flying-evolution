#include <cmath>
#include <iostream>
#include <memory>
#include <random>
#include "sound.hxx"
#include "generator.hxx"

typedef std::int16_t Value;

struct __attribute__((packed)) Sample
{
	Value left;
	Value right;
};

static long const buf_length = 4096;
static double const max_latency = 0.3;

static SDL_AudioDeviceID dev;
long sample_rate;
long max_latency_size;

std::array<Sample,  buf_length> buffer;
std::ranlux24 main_rnd;
NoiseGenerator2 *gen_left;
NoiseGenerator2 *gen_right;

void initAudio()
{
	SDL_AudioSpec want, have;
	std::cout << "Initializing audio" << std::endl;
	std::memset(&want, 0, sizeof(want));
	want.freq = 48000;
	want.format = AUDIO_S16SYS;
	want.channels = 2;
	want.samples = buf_length;
	want.callback = nullptr;

	dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	if(!dev)
		throw std::runtime_error("Failed to open audio");
	sample_rate = have.freq;
	std::cout << "Audio sample rate: " << sample_rate << std::endl;
	max_latency_size = sizeof(Sample) * sample_rate * max_latency;

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
	if(SDL_GetQueuedAudioSize(dev) > max_latency_size)
		return; // have to drop...
	gen_left->power = std::abs(left);
	gen_right->power = std::abs(right);
	long samples = sample_rate * time;
	while(samples > 0)
	{
		long count = std::min(samples, buf_length);
		for(long k = 0; k != count; ++k)
		{
			buffer[k].left = static_cast<Value>(32000.0 * gen_left->next());
			buffer[k].right = static_cast<Value>(32000.0 * gen_right->next());;
		}
		if(SDL_QueueAudio(dev, buffer.data(), sizeof(Sample) * count))
			throw std::runtime_error("Can't queue the audio");
		samples -= count;
	}
}
