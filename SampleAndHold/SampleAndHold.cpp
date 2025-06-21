#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod hw;

// Stores the value of potentiometer 1
float knob1 = 0.0f;

// The maximum value for the number of samples to be held
float maximum_samples_held = 64;

// The incrementor that carries what the index of the current sample
int current_hold_time = 0;

// Stores the number of samples to be held
int hold_value = 0;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	// Create the static variable that holds the signal every hold time
	static float signal = in[0][0];

	
	// Loop through and assign the held value
	for (size_t i = 0; i < size; i++)
	{
		// If the current_hold_time exceeds the hold value, reset the counter and set the signal
		if (current_hold_time >= hold_value) {
			current_hold_time = 0;
			signal = in[0][0];
		// Else increment the current index that indicates how many times the sample is held 
		} else {
			current_hold_time++;
		}

		// Assign the held value to the output
		out[0][i] = signal;
		out[1][i] = signal;
	}
}

int main(void)
{
	// Initialize the Electrosmith Daisy Pod
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAdc();
	hw.StartAudio(AudioCallback);

	while(1) {
		// Get the value of knob 1 and store it in its variable
		hw.ProcessAnalogControls();
		knob1 = hw.knob1.Value();

		// Cast the number of samples to be held to an int and set the global variable
		hold_value = (int)(knob1 * maximum_samples_held);
		if (hold_value == 0) {
			hold_value = 1;
		}
	}
}
