#include "state.h"
#include "emulator.h"
#include "platform.h"
#include "renderer.h"

#include <stdint.h>

uint32_t rgb_to_hex(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}