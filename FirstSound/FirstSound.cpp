#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

static DaisyPod hw;
static Oscillator osc;

void NextSamples(float &signal) {
	signal = osc.Process();
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	hw.ProcessAllControls();

	float s;
	NextSamples(s);

	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = s;
		out[1][i] = s;
	}
}

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	float sample_rate;
	sample_rate = hw.AudioSampleRate();

	osc.Init(sample_rate);
	osc.SetFreq(440);
	osc.SetAmp(1);
	osc.SetWaveform(osc.WAVE_SAW);

	hw.StartAdc();
	hw.StartAudio(AudioCallback);
	while(1) {}
}