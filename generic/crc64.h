
#ifndef CRC64_H
#define CRC64_H
#include <stdint.h>
#include <stdio.h>

extern uint64_t lzma_crc64(const uint8_t *buf, size_t size, uint64_t crc);

#endif
