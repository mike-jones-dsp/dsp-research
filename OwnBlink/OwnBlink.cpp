#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

#define DELAY_TIME 2500

DaisyPod hw;

int main(void)
{
	hw.Init();
	while(1) {
		hw.led1.Set(0, 1, 1);
		hw.UpdateLeds();

		System::Delay(DELAY_TIME);

		hw.led1.Set(1, 0, 1);
		hw.UpdateLeds();

		System::Delay(DELAY_TIME);

		hw.led1.Set(1, 1, 0);
		hw.UpdateLeds();

		System::Delay(DELAY_TIME);
	}
}
