#include <assert.h>
#include <list.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "body.h"
#include "color.h"
#include "polygon.h"
#include "sdl_wrapper.h"

const double MIN_XY = 0;
const double MAX_X = 1000;
const double MAX_Y = 500;
const double OUTER_RADIUS = 50;
const double INNER_MULT = 0.382;
const double GRAV = -100;
const double H_VELOCITY = 75;
const double V_VELOCITY = 0;
const double DAMPENER_MIN = 0.75;
const double DAMPENER_MAX = 0.93;
const double ROT_ANGLE = 75;
const double PERIOD = 1.3;
const double INIT_X = 50;
const double INIT_Y = 450;
const int INIT_SIDES = 2;
const double LIST_SIZE = 10;
const double BODY_MASS = 100;
const double FULL_CIRCLE = 360;
const float COLOR_CONST = 1 / (float)4;

double ang_to_rad(double angle) {
    return angle * 2 * M_PI / FULL_CIRCLE;
}

list_t *make_star(vector_t center, int num_sides) {
    int star_size = num_sides * 2;
    list_t *star = list_init(star_size, free);
    double interior_angle = FULL_CIRCLE / num_sides;

    for (size_t i = 0; i < star_size; i++) {
        vector_t *v = malloc(sizeof(*v));
        if (i == 0) {
            *v = (vector_t){0, OUTER_RADIUS};
        } else if (i % 2 == 1) {
            *v = vec_multiply(INNER_MULT, vec_rotate(*((vector_t *)list_get(star, i - 1)),
                                                     ang_to_rad(interior_angle / 2)));
        } else {
            *v = vec_rotate(*((vector_t *)list_get(star, i - 2)),
                            ang_to_rad(interior_angle));
        }
        list_add(star, v);
    }
    polygon_translate(star, center);
    return star;
}

void velocity_check(body_t *star, double dt) {
    vector_t velocity = body_get_velocity(star);
    list_t *vertices = body_get_shape(star);
    double dampener =
        (double)rand() / (double)(RAND_MAX) * (DAMPENER_MAX - DAMPENER_MIN) +
        DAMPENER_MIN;
    int add = 2;
    if (list_size(vertices) ==
        4) { // Handles edge case of 4 sided figure's inner vertices
        add = 1;
    }
    for (size_t i = 0; i < list_size(vertices); i += add) {
        vector_t v =
            vec_add(*((vector_t *)list_get(vertices, i)), vec_multiply(dt, velocity));
        if (v.y < MIN_XY) {
            velocity.y = -velocity.y * dampener;
        }
    }
    body_set_velocity(star, velocity);
}

int boundary_check(body_t *star) {
    int edges_over = 0;
    list_t *vertices = body_get_shape(star);
    for (size_t i = 0; i < list_size(vertices); i += 2) {
        vector_t v = *((vector_t *)list_get(vertices, i));
        if (v.x > MAX_X) {
            edges_over++;
        }
    }
    if (edges_over == list_size(vertices) / 2) {
        return 1;
    }
    return 0;
}

body_t *star_pos(body_t *star, double dt) {
    body_add_force(star, (vector_t){0, GRAV * body_get_mass(star)});
    body_tick(star, dt);
    return star;
}

rgb_color_t change_color(body_t *shape) {
    rgb_color_t color = body_get_color(shape);
    float r = color.r;
    float g = color.g;
    float b = color.b;
    if (r == 1 && g < 1 && b == 0) {
        g += COLOR_CONST;
    }
    if (g == 1 && r > 0 && b == 0) {
        r -= COLOR_CONST;
    }
    if (g == 1 && b < 1 && r == 0) {
        b += COLOR_CONST;
    }
    if (b == 1 && g > 0 && r == 0) {
        g -= COLOR_CONST;
    }
    if (b == 1 && r < 1 && g == 0) {
        r += COLOR_CONST;
    }
    if (r == 1 && b > 0 && g == 0) {
        b -= COLOR_CONST;
    }
    return (rgb_color_t){r, g, b};
}

int main() {
    sdl_init((vector_t){MIN_XY, MIN_XY}, (vector_t){MAX_X, MAX_Y});
    vector_t center = {INIT_X, INIT_Y};
    int num_sides = INIT_SIDES;
    scene_t *shapes = scene_init();
    list_t *vertices = make_star(center, num_sides);
    body_t *star = body_init(vertices, BODY_MASS, (rgb_color_t){1, 0, 0});
    body_set_velocity(star, (vector_t){H_VELOCITY, V_VELOCITY});
    scene_add_body(shapes, star);
    double time = 0.0;
    int start = 0;
    double rotate = 0.0;

    while (!sdl_is_done()) {
        double dt = time_since_last_tick();
        time += dt;
        rotate += dt;
        if (time > PERIOD) {
            num_sides++;
            list_t *vertices = make_star(center, num_sides);
            body_t *temp = scene_get_body(shapes, num_sides - 3);
            rgb_color_t color = change_color(temp);
            body_t *star = body_init(vertices, BODY_MASS, color);
            body_set_velocity(star, (vector_t){H_VELOCITY, V_VELOCITY});
            scene_add_body(shapes, star);
            time = 0.0;
        }
        for (size_t i = start; i < scene_bodies(shapes); i++) {
            body_t *shape = scene_get_body(shapes, i);
            velocity_check(shape, dt);
            shape = star_pos(shape, dt);
            body_set_rotation(shape, -ang_to_rad(ROT_ANGLE) * rotate);
            start += boundary_check(shape);
        }
        sdl_render_scene(shapes);
    }
    scene_free(shapes);
}
