#include "daisy_pod.h"
#include "daisysp.h"

// Namespaces used in this project
using namespace daisy;
using namespace daisysp;

// Objects used
DaisyPod hw;
Oscillator osc;

Oscillator fm_osc;

AdEnv ad;

// The number of different oscillator types
const uint8_t waveform_count = 4;

// All the notes that it is possible to play
float possible_notes[] = {220, 246.94, 261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25, 587.33, 659.25, 698.46, 783.99, 880};

// This variable stores the main oscillator's frequency
float oscillator_frequency = possible_notes[0];

// These correspond to the two potentiometers values
float fm_knob = 0.0f;
float fm_freq_knob = 0.0f;

// The maixmum amount +- that the FM oscilator can change the main oscillator's frequency by
const float fm_amount = 50.0f;

// The maximum amount that the FM Oscillator's Frequency can be
const float fm_freq_amount = 250.0f;

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
	// First set the frequency for the FM Input
	fm_osc.SetFreq(fm_freq_amount * fm_freq_knob);

	// The main oscillator's frequency is a combination of the note frequency plus the frequency change introduced
	// by the FM Oscillator

	osc.SetFreq(oscillator_frequency + fm_osc.Process() * fm_knob * fm_amount);

	// Process the current Attack Decay Envelope Sample
	float ad_this_sample = ad.Process();

	// Process the oscillator signal
	signal = osc.Process();

	// Multiply the oscillators signal by the attack decay envelope's sample to get the output of the synth
	signal *= ad_this_sample;
}

// Choose a random note
void UpdateNote() {
	int random_note = rand() % 15;
	oscillator_frequency = possible_notes[random_note];
	//osc.SetFreq(possible_notes[random_note]);
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

	// Intialize the FM Osc
	fm_osc.Init(sample_rate);
	fm_osc.SetFreq(50);
	fm_osc.SetAmp(1);
	fm_osc.SetWaveform(fm_osc.WAVE_TRI);

	// The Attack Decay Envelope's initialization
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
			
			// Set the changed waveform
			osc.SetWaveform(tambers[current_waveform]);
		}
		
		if (encoder_change < 0) {
			// Its an unsigned int so make sure it doesn't go into the negatives
			if (current_waveform <= 0) {
				current_waveform = waveform_count - 1;
			} else {
				current_waveform--;
			}

			// If the waveform has changed, set it
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
		
		// When the button is just pressed, trigger the AD envelope
		if (hw.button1.RisingEdge()) {
			if (!ad.IsRunning()) {
				ad.Trigger();
			}

			// Pick a new random note
			UpdateNote();
		}

		// Set the two values corresponding to knob1 and knob2
		hw.ProcessAnalogControls();
		fm_knob = hw.knob1.Process();
		fm_freq_knob = hw.knob2.Process();

		// Update the status of the LEDs
		hw.UpdateLeds();
	}
}