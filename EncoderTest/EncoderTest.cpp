#include "daisy_pod.h"

using namespace daisy;

DaisyPod hw;

// Holds the three colors used in this example
Color colors[3];

// Used to hold the encoder increment value
static int32_t encoder_value;

// What color the leds are currently set too
int8_t led1_color = 0;
int8_t led2_color = 0;

// The toggle variable that chooses between the LEDs
static bool current_led = true;

int main(void)
{
	// Initialize the Daisy Pod
	hw.Init();

	// Set the initial colors of the LEDs and update their values
	hw.led1.Set(0, 1, 0);
	hw.led2.Set(0, 0, 1);
	hw.UpdateLeds();

	// Sets all the colors used in this project
	colors[0].Init(Color::PresetColor::RED);
	colors[1].Init(Color::PresetColor::GREEN);
	colors[2].Init(Color::PresetColor::BLUE);

	while(1) {
		// Debounce and store the value of the encoder
		hw.encoder.Debounce();
		encoder_value = hw.encoder.Increment();

		// If the encoder has been turned clockwise
		if (encoder_value > 0)
		{
			// increment the led color depending on which led is selected
			if (current_led)
			{
				led1_color++;

				// If the color variable is out of range, reset it to 0
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

		// If the encoder has been turned counter clockwise
		else if (encoder_value < 0)
		{
			if (current_led)
			{
				
				// Make sure it isn't less than 0
				if (led1_color <= 0)
				{
					led1_color = 2;
				}
				else
				{
					led1_color--;
				}
			}
			else
			{
				if (led2_color <= 0)
				{
					led2_color = 2;
				}
				else
				{
					led2_color--;
				}
			}
		}

		// Process the buttons input
		hw.ProcessDigitalControls();
		bool button_one_just_pressed = hw.button1.RisingEdge();
		bool button_two_just_pressed = hw.button2.RisingEdge();

		// If button one is pressed, change which LED is selected
		if (button_one_just_pressed)
		{
			current_led = !current_led;
		}

		// If button two is pressed, set the color as blue
		if (button_two_just_pressed)
		{
			led1_color = 2;
			led2_color = 2;
		}

		// Attempting to get the encoder push value
		// hw.encoder.Debounce();
		// bool encoder_just_pressed = hw.encoder.Pressed();
		// if (encoder_just_pressed)
		// {
		// 	led1_color = 0;
		// 	led2_color = 0;
		// }

		// Set and update the colors of the LEDs
		hw.led1.SetColor(colors[led1_color]);
		hw.led2.SetColor(colors[led2_color]);
		hw.UpdateLeds();
		}
}
