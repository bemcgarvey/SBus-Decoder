#include "sbus_settings.h"

uint8_t calcChecksum(uint8_t *buff) {
    uint8_t len = sizeof(Settings);
    uint8_t chksum = 0xaa;
    while (len > 0) {
        chksum ^= *buff;
        ++buff;
        --len;
    }
    return chksum;
}
