#include <stdint.h>

struct zoe_m8q_lib {
    // Default: 0x42
    // Can be changed by setting `mode` field in `UBX-CFG-PORT` for DDC
    uint8_t (*default_i2c_addr)(void);
};


extern const struct zoe_m8q_lib ZOE_M8Q_Lib;
