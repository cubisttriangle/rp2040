#include <stdio.h>
#include "pico/stdlib.h"
#include "i2c.h"
#include "zoe-m8q.h"

int main() {
    stdio_init_all();

    if (!I2C_Lib.init()) {
        printf("I2C init failed");
        return 1;
    }

    uint8_t m8q_addr = ZOE_M8Q_Lib.default_i2c_addr();

    while (1) {
        bool zoe_m8q_available = I2C_Lib.device_present_at(m8q_addr);
        if (zoe_m8q_available) {
            printf("ZOE_M8Q available on 0x%02x\n", m8q_addr);
        } else {
            I2C_Lib.scan_bus();
        }
        sleep_ms(3000);
    }

    return 0;
}

