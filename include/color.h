#ifndef __COLOR_H__
#define __COLOR_H__
#include <stdlib.h>

/**
 * A color to display on the screen.
 * The color is represented by its red, green, and blue components.
 * Each component must be between 0 (black) and 1 (white).
 */
typedef struct {
    float r;
    float g;
    float b;
} rgb_color_t;

// Makes the brick color lighter to account for health
rgb_color_t lighter_color(rgb_color_t color);

#endif // #ifndef __COLOR_H__
