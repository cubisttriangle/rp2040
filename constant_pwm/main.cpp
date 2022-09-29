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
    pwm_set_chan_level(slice, channel, 1500);
    pwm_set_enabled(slice, true);

    return 0;
}
