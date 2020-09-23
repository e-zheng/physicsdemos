#ifndef __DEMO_BREAKOUT_UTIL_H__
#define __DEMO_BREAKOUT_UTIL_H__

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "body.h"
#include "vector.h"
#include "list.h"

body_t *make_rect(double width, double height, rgb_color_t color, int player);

body_t *circle_sector_breakout(double radius);

#endif // #ifndef __DEMO_BREAKOUT_UTIL_H__
