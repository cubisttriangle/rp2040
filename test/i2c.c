#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "i2c.h"

bool init() {
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
#warning i2c/bus_scan example requires a board with I2C pins
    puts("Default I2C pins were not defined");
    return false;
#else
    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    return true;
#endif
}

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool addr_is_reserved(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

bool device_present_at(uint8_t addr) {
    if (addr_is_reserved(addr)) {
        printf("ERROR: device_is_present_at_addr(%02x) - reserved address\n", addr);
        return false;
    } else {
        uint8_t rx_data;
        return i2c_read_blocking(i2c_default, addr, &rx_data, 1, false) >= 0;
    }
}

void scan_bus() {
    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (addr_is_reserved(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
}

int write_blocking(uint8_t addr, const uint8_t* buf, size_t len, bool no_stop) {
    return i2c_write_blocking(i2c_default, addr, buf, len, no_stop);
}

int read_blocking(uint8_t addr, uint8_t* buf, size_t len, bool no_stop) {
    return i2c_read_blocking(i2c_default, addr, buf, len, no_stop);
}

size_t get_read_available() {
    return i2c_get_read_available(i2c_default);
}

const struct i2c_lib I2C_Lib = {
    .init = init,
    .addr_is_reserved = addr_is_reserved,
    .device_present_at = device_present_at,
    .scan_bus = scan_bus,
    .write_blocking = write_blocking,
    .read_blocking = read_blocking,
    .get_read_available = get_read_available
};
