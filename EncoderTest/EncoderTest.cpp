#include "daisy_pod.h"

using namespace daisy;

DaisyPod hw;

// void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
// {
// 	hw.ProcessAllControls();
// 	for (size_t i = 0; i < size; i++)
// 	{
// 		out[0][i] = in[0][i];
// 		out[1][i] = in[1][i];
// 	}
// }
Color colors[3];
static int32_t encoder_value;

int8_t led1_color = 0;
int8_t led2_color = 0;

static bool current_led = true;

int main(void)
{
	hw.Init();

	hw.led1.Set(0, 1, 0);
	hw.led2.Set(0, 0, 1);
	hw.UpdateLeds();

	colors[0].Init(Color::PresetColor::RED);
	colors[1].Init(Color::PresetColor::GREEN);
	colors[2].Init(Color::PresetColor::BLUE);

	while(1) {
		// hw.ProcessDigitalControls();
		hw.encoder.Debounce();
			encoder_value = hw.encoder.Increment();

			if (encoder_value > 0)
			{
				if (current_led)
				{
					led1_color++;

					if (led1_color > 2)
					{
						led1_color = 0;
					}
				}
				else
				{
					led2_color++;

					if (led2_color > 2)
					{
						led2_color = 0;
					}
				}
			} 
			else if (encoder_value < 0)
			{
				if (current_led)
				{
					led1_color--;

					if (led1_color < 0)
					{
						led1_color = 2;
					}
				}
				else
				{
					led2_color--;

					if (led2_color < 0)
					{
						led2_color = 2;
					}
				}
			}

			hw.ProcessDigitalControls();
			bool button_one_just_pressed = hw.button1.RisingEdge();
			bool button_two_just_pressed = hw.button2.RisingEdge();

			if (button_one_just_pressed)
			{
				current_led = !current_led;
			}

			if (button_two_just_pressed)
			{
				led1_color = 2;
				led2_color = 2;
			}

			hw.encoder.Debounce();
			bool encoder_just_pressed = hw.encoder.Pressed();
			if (encoder_just_pressed)
			{
				led1_color = 0;
				led2_color = 0;
			}

			hw.led1.SetColor(colors[led1_color]);
			hw.led2.SetColor(colors[led2_color]);

			hw.UpdateLeds();
		}
}
