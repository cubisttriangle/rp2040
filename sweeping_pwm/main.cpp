#include "pico/stdlib.h"
#include "hardware/pwm.h"

int main()
{
    // Set GPIO0 and GPIO1 to PWM
    gpio_set_function(0, GPIO_FUNC_PWM);
    gpio_set_function(1, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to GPIO0.
    uint slice = pwm_gpio_to_slice_num(0);
    uint channel = PWM_CHAN_A;

    // 125 MHz / 125 = 1KHz
    // Now we have 1us / period
    pwm_set_clkdiv(slice, 125);
    pwm_set_wrap(slice, 10000);
    pwm_set_enabled(slice, true);

    int16_t dir = 1;
    uint16_t step = 1;
    constexpr uint32_t sleepTimeMs = 1;
    uint16_t pulseWidthMin = 1000;
    uint16_t pulseWidthMax = 2000;
    uint16_t i = pulseWidthMin;

    while (1)
    {
        pwm_set_chan_level(slice, channel, i);
        sleep_ms(sleepTimeMs);
	if (i == pulseWidthMin && dir == -1)
	{
	    dir = 1;
	}
	else if (i == pulseWidthMax && dir == 1)
	{
	    dir = -1;
	}
	i += dir * step;
    }

    return 0;
}
