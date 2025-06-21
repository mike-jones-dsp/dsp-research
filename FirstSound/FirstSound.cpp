#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

// The objects used in this simple sound example
static DaisyPod hw;
static Oscillator osc;

// Get the next sample by passing by reference
void NextSamples(float &signal) {
	signal = osc.Process();
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	// Used to store the next sample
	float s;

	// Loop through the buffer
	for (size_t i = 0; i < size; i++)
	{
		// Get the next sample and assign it to the outputs
		NextSamples(s);
		out[0][i] = s;
		out[1][i] = s;
	}
}

int main(void)
{
	// Initialize the Daisy Pod
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	// Get the sample rate
	float sample_rate;
	sample_rate = hw.AudioSampleRate();

	// Initialize the oscillator
	osc.Init(sample_rate);
	osc.SetFreq(440);
	osc.SetAmp(1);
	osc.SetWaveform(osc.WAVE_SAW);

	// Initialize the ADC
	hw.StartAdc();

	// Start the audio input and output buffer
	hw.StartAudio(AudioCallback);
	while(1) {}
}