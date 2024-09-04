#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

struct Color {
    Color() = default;

    Color(uint8_t blue, uint8_t green, uint8_t red);

    uint8_t blue = 0;
    uint8_t green = 0;
    uint8_t red = 0;

    void SetVals(uint8_t in_blue, uint8_t in_green, uint8_t in_red);
};
#endif