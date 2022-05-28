#include <stdint.h>

struct i2c_lib {
    bool (*init)(void);
    bool (*addr_is_reserved)(uint8_t);
    bool (*device_present_at)(uint8_t);
    void (*scan_bus)(void);
    int  (*write_blocking)(uint8_t, const uint8_t* buf, size_t len, bool no_stop);
    int  (*read_blocking)(uint8_t, uint8_t* buf, size_t len, bool no_stop);
    size_t (*get_read_available)(void);
};

extern const struct i2c_lib I2C_Lib;
