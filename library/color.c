#include "color.h"

const double MULT = 0.5;

rgb_color_t lighter_color(rgb_color_t color){
    float new_r = color.r + MULT * (1 - color.r);
    float new_g = color.g + MULT * (1 - color.g);
    float new_b = color.b + MULT * (1 - color.b);
    rgb_color_t result = (rgb_color_t){new_r, new_g, new_b};
    return result;
}
