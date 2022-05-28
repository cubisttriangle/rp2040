#include "zoe-m8q.h"

uint8_t default_i2c_addr() {
    return 0x42;
}

const struct zoe_m8q_lib ZOE_M8Q_Lib = {
    .default_i2c_addr = default_i2c_addr
};
