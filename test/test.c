#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    sleep_ms(10000);

    while (1) {

        bool zoe_m8q_available = I2C_Lib.device_present_at(m8q_addr);
        if (zoe_m8q_available) {
            printf("ZOE_M8Q available on 0x%02x\n", m8q_addr);

            // Make request
            // Poll 0xFD (hi) + 0xFE (lo) for bytes available
            // When bytes available > 0, read n bytes from 0xFF reg
            // If the first byte from 0xFF reg is 0xFF, this is invalid

            // DDC Random Read Access...
            // Following the START condition from master, the 7-bit device
            // address and the RW bit (which is logic low for write access)
            // are clocked onto the bus by the master. The receiver answers
            // with ACK (logic low) to indicate that it recognizes the address.
            // Next, the 8-bit address of the register to be read must be
            // written to the bus. Following the receiver's ACK, the master
            // again triggers a start condition and writes the device address,
            // but this time the RW bit is logic high to initiate read access.
            // Now the master can read 1 to N bytes from the receiver,
            // generating a NOT-ACK and a STOP condition after the last byte
            // is read.
            // START + ADDR+W, ACK, REG_ADDR, ACK
            // START + ADDR+R, ACK, READ, ..., STOP

            // DDC Write Access (only available for UBX and NMEA)...
            // Following the START condition from the master, the 7-bit
            // address and the RW bit (which is logic low for write access)
            // are clocked onto the bus by the master. The receiver answers
            // with ACK (logic low) to indicate that it is responsible for
            // the given address. Now, the master can write 2 to N bytes to
            // the receiver, generating a STOP condition after the last byte
            // is written. The nubmer of data bytes must be at least 2 to
            // properly distinguish from the write access to set the address
            // counter in random read accesses.
            // START + ADDR+W, ACK, BYTE1, BYTE2, ..., BYTEN, STOP

            // UBX protocol...
            // SYNC1 (0xB5), SYNC2 (0x62), CLASS, ID, LENGTH, PAYLOAD, CK_A, CK_B
            // Preamble: SYNC1 + SYNC2
            // CLASS: 1-byte message class
            // ID: 1-byte message ID
            // LENGTH: 2-byte little-endian length of UBX message payload only
            // Checksum: CK_A + CK_B, calculated (inclusive) CLASS <-> PAYLOAD

            uint8_t mon_ver_req[8];
            mon_ver_req[0] = 0xB5; // SYNC1
            mon_ver_req[1] = 0x62; // SYNC2
            mon_ver_req[2] = 0x0A; // CLASS
            mon_ver_req[3] = 0x04; // ID
            mon_ver_req[4] = 0x00; // PAYLOAD LENGTH (HIGH)
            mon_ver_req[5] = 0x00; // PAYLOAD LENGTH (LOW)
            mon_ver_req[6] = 0x00; // CK_A
            mon_ver_req[7] = 0x00; // CK_B

            // Calculate checksum
            int checksum_payload_len = 0;
            uint8_t* checksum_payload = &mon_ver_req[2];
            uint8_t* ck_a = &mon_ver_req[6];
            uint8_t* ck_b = &mon_ver_req[7];
            *ck_a = 0;
            *ck_b = 0;
            for (int i = 0; i < checksum_payload_len; ++i) {
                *ck_a = *ck_a + checksum_payload[i];
                *ck_b = *ck_b + *ck_a;
            }

            // Write UBX-MON-VER request
            const uint8_t* buf = mon_ver_req;
            int ret = I2C_Lib.write_blocking(m8q_addr, buf, 8, false);
            printf("Wrote %d bytes of mon_ver_req\n", ret);

            while (1) {
                // Try reading the data available regs...
                uint8_t available_hi_reg = 0xFD;
                // Set read register pointer
                ret = I2C_Lib.write_blocking(m8q_addr, &available_hi_reg, 1, true);
                if (ret != 1) {
                    printf("Error setting read register to 0x%02x: %d\n",
                           available_hi_reg, ret);
                    break;
                }
                uint8_t bytes_available[2];
                bytes_available[0] = 0;
                bytes_available[1] = 0;
                ret = I2C_Lib.read_blocking(m8q_addr, bytes_available, 2, false);
                if (ret != 2) {
                    printf("Error reading bytes-available reg: %d\n", ret);
                    break;
                } else {
                    uint16_t bytes_to_read = bytes_available[0];
                    bytes_to_read = (bytes_to_read << 8) | bytes_available[1];
                    //printf("Bytes available: 0x%02x (hi) | 0x%02x (lo) = 0x%04x\n",
                    //        bytes_available[0], bytes_available[1], bytes_to_read);

                    if (bytes_to_read > 0) {
                        printf("Attempting to read %d bytes...\n", bytes_to_read);

                        // get_read_available always returns zero
                        size_t read_available = I2C_Lib.get_read_available();
                        read_available = bytes_to_read;
                        //printf("RP2040 can read %zu bytes in one shot\n", read_available);

                        if (bytes_to_read <= read_available) {
                            //printf("Should be able to read everything at once\n");
                            uint8_t data_reg = 0xFF;
                            // Set read register pointer
                            ret = I2C_Lib.write_blocking(m8q_addr, &data_reg, 1, true);
                            if (ret != 1) {
                                printf("Error setting read register to 0x%02x: %d\n", data_reg, ret);
                                break;
                            }
                            uint8_t* buf = malloc(bytes_to_read + 1);
                            memset(buf, 0, bytes_to_read + 1);
                            ret = I2C_Lib.read_blocking(m8q_addr, buf, bytes_to_read, false);
                            if (ret != bytes_to_read) {
                                printf("ERROR: expect to read %d bytes, but only read %d\n",
                                        bytes_to_read, ret);
                            } else {
                                printf("\n%s\n\n", buf);
                            }
                            free(buf);
                        } else {
                            printf("Can't read everything at once\n");
                        }
                        break;
                    } else {
                        sleep_ms(100);
                        continue;
                    }
                }
            }
        } else {
            I2C_Lib.scan_bus();
        }
    }

    return 0;
}

