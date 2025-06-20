#include "daisy_pod.h"
#include "daisysp.h"

// Namespaces used in this project
using namespace daisy;
using namespace daisysp;

// Objects used
DaisyPod hw;
Oscillator osc;
AdEnv ad;

// The number of different oscillator types
const uint8_t waveform_count = 4;

// All the notes that it is possible to play
float possible_notes[] = {220, 246.94, 261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25, 587.33, 659.25, 698.46, 783.99, 880};

// Different osc types
uint8_t tambers[] = {
	Oscillator::WAVE_SIN,
	Oscillator::WAVE_SAW,
	Oscillator::WAVE_SQUARE,
	Oscillator::WAVE_TRI,
};

// Used to store the waveform used in the oscillator
uint8_t current_waveform = tambers[0];

// Holds all the colors used to indicate the waveforms
Color waveform_colors[waveform_count];

// Does the math for the next samples, passed as reference to save time since it is called so often
void NextSamples(float &signal) {
	float ad_this_sample = ad.Process();
	signal = osc.Process();
	signal *= ad_this_sample;
}

// Choose a random note
void UpdateNote() {
	int random_note = rand() % 15;
	osc.SetFreq(possible_notes[random_note]);
}

// The function used to process the audio
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	float s;

	// Loop through and assign both channels the right samples
	for (size_t i = 0; i < size; i++)
	{
		NextSamples(s);

		out[0][i] = s;
		out[1][i] = s;
	}
}

int main(void)
{
	// Stores the sample rate
	float sample_rate;

	// Initialize the Daisy Pod
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	sample_rate = hw.AudioSampleRate();

	// Initialize the oscillators
	osc.Init(sample_rate);
	osc.SetFreq(possible_notes[2]);
	osc.SetAmp(1);
	osc.SetWaveform(osc.WAVE_SAW);

	ad.Init(sample_rate);
	ad.SetTime(ADENV_SEG_ATTACK, 0.25);
	ad.SetTime(ADENV_SEG_DECAY, 1.5);
	ad.SetMax(1);
	ad.SetMin(0);
	ad.SetCurve(0);

	// Start the ADC and the aduio processing
	hw.StartAdc();
	hw.StartAudio(AudioCallback);

	// Set the different colors to indicate which waveform is chosen
	waveform_colors[0].Init(Color::PresetColor::BLUE);
	waveform_colors[1].Init(Color::PresetColor::GREEN);
	waveform_colors[2].Init(Color::PresetColor::RED);
	waveform_colors[3].Init(Color::PresetColor::GOLD);
	
	// Loop until done
	while(1) {
		// Check to see if the encoder has been changed
		hw.encoder.Debounce();
		int encoder_change = hw.encoder.Increment();

		// If the encoder has been changed, set the right waveform
		if (encoder_change > 0) {
			current_waveform++;

			if (current_waveform >= waveform_count) {
				current_waveform = 0;
			}
			
			osc.SetWaveform(tambers[current_waveform]);
		}
		
		if (encoder_change < 0) {
			if (current_waveform == 0) {
				current_waveform = waveform_count - 1;
			} else {
				current_waveform--;
			}



			osc.SetWaveform(tambers[current_waveform]);
		}

		// Sets the color of the waveform LED indicator
		hw.led2.SetColor(waveform_colors[current_waveform]);

		// Process the buttons that are used to choose a random note
		hw.ProcessDigitalControls();
		if (hw.button1.Pressed()) {
			ad.Trigger();
			hw.led1.Set(0, 0, 1);
		} else {
			hw.led1.Set(0, 0, 0);
		}
		
		if (hw.button1.RisingEdge()) {
			if (!ad.IsRunning()) {
				ad.Trigger();
			}

			UpdateNote();
		}

		// Update the status of the LEDs
		hw.UpdateLeds();
	}
}