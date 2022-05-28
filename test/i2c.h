struct i2c_lib {
    bool (*init)(void);
    bool (*addr_is_reserved)(uint8_t);
    bool (*device_present_at)(uint8_t);
    void (*scan_bus)(void);
};

extern const struct i2c_lib I2C_Lib;
