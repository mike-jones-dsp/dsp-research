#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod hw;

float lastIn[2] = {0.0, 0.0};

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	hw.ProcessAllControls();
	for (size_t i = 0; i < size; i++)
	{
		// The filter is a FIR filter with a0 of 0.5 and a a1 of 0.5
		out[0][i] = 0.5 * in[0][i] * 0.5 * lastIn[0];
		out[1][i] = 0.5 * in[1][i] * 0.5 * lastIn[1];

		lastIn[0] = in[0][i];
		lastIn[1] = in[1][i]; 
	}
}

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAdc();
	hw.StartAudio(AudioCallback);
	while(1) {}
}
