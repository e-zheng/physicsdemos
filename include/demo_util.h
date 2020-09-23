#ifndef __DEMO_UTIL_H__
#define __DEMO_UTIL_H__

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "body.h"
#include "vector.h"
#include "list.h"

body_t *make_char(size_t rad, double min_rad, double max_rad, double scale1, double scale2);

body_t *make_bullet(body_t *maker);

#endif // #ifndef __DEMO_UTIL_H__
