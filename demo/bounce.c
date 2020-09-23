#include <math.h>
#include <stdlib.h>
#include "color.h"
#include "polygon.h"
#include "sdl_wrapper.h"

const double minXY = 0.0;
const int maxX = 1000;
const int maxY = 500;
const int red = 0;
const int green = 0;
const int blue = 1;
const int star_sides = 10;
const double outer_radius = 100;
const double inner_mult = 0.382;
const double star_angle_one = 36;
const double half_circle = 180;

list_t *make_star(vector_t center) {
    list_t *star = list_init(star_sides, free);

    for (int i = 0; i < star_sides; i++) {
        vector_t *v = malloc(sizeof(*v));
        if (i == 0) {
            *v = (vector_t){0, outer_radius};
        } else if (i % 2 == 1) {
            *v =
                vec_multiply(inner_mult, vec_rotate(*((vector_t *)list_get(star, i - 1)),
                                                    star_angle_one * M_PI / half_circle));
        } else {
            *v = vec_rotate(*((vector_t *)list_get(star, i - 2)),
                            2 * star_angle_one * M_PI / half_circle);
        }
        list_add(star, v);
    }
    polygon_translate(star, center);
    return star;
}

vector_t velocity_check(list_t *star, double dt, vector_t velocity) {
    for (int i = 0; i < star_sides; i += 2) {
        vector_t v =
            vec_add(*((vector_t *)list_get(star, i)), vec_multiply(dt, velocity));
        if (v.x > maxX || v.x < minXY) {
            velocity.x = -velocity.x;
        }
        if (v.y > maxY || v.y < minXY) {
            velocity.y = -velocity.y;
        }
    }
    return velocity;
}
list_t *star_pos(list_t *star, double dt, vector_t velocity) {
    polygon_translate(star, vec_multiply(dt, velocity));
    polygon_rotate(star, maxX * M_PI / half_circle * dt, polygon_centroid(star));
    return star;
}

int main() {
    sdl_init((vector_t){minXY, minXY}, (vector_t){maxX, maxY});
    vector_t center = {outer_radius, outer_radius};
    list_t *star = make_star(center);
    vector_t velocity = {maxX, maxX};
    while (!sdl_is_done()) {
        double dt = time_since_last_tick();
        velocity = velocity_check(star, dt, velocity);
        star = star_pos(star, dt, velocity);
        sdl_clear();
        sdl_draw_polygon(star, (rgb_color_t){(float)0, (float)1.0, (float)1});
        sdl_show();
    }
    list_free(star);
}
