#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

// The delay time
#define DELAY_TIME 2500

DaisyPod hw;

int main(void)
{
	// Initialize the Daisy Pod
	hw.Init();

	while(1) {
		// Set and then update the LEDs
		hw.led1.Set(0, 1, 1);
		hw.UpdateLeds();

		// Wait the amount of delay time...
		System::Delay(DELAY_TIME);

		// ... then repeat
		hw.led1.Set(1, 0, 1);
		hw.UpdateLeds();

		System::Delay(DELAY_TIME);

		hw.led1.Set(1, 1, 0);
		hw.UpdateLeds();

		System::Delay(DELAY_TIME);
	}
}
