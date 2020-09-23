#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "aux.h"
#include "body.h"
#include "color.h"
#include "force_storage.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "test_util.h"
#include "vector.h"

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 500
const double MIN_XY = 0;
const double MAX_X = 1000;
const double MAX_Y = 500;
const int NUM_BODIES = 50;
const int BODY_SIDES = 4;
const double MAX_RADIUS = 20;
const double MIN_RADIUS = 10;
const double INNER_MULT = 0.382;
const double MASS = 50.0;
const double GRAV = 750.0;
const double CIRCLE_ANGLE = 360.0;

double ang_to_rad(double angle) {
    return angle * 2 * M_PI / CIRCLE_ANGLE;
}

body_t *make_body(int num_sides, double radius) {
    int star_size = num_sides * 2;
    list_t *star = list_init(star_size, free);
    double interior_angle = CIRCLE_ANGLE / num_sides;
    for (int i = 0; i < star_size; i++) {
        vector_t *v = malloc(sizeof(*v));
        if (i == 0) {
            *v = (vector_t){0, radius};
        } else if (i % 2 == 1) {
            *v = vec_multiply(INNER_MULT, vec_rotate(*((vector_t *)list_get(star, i - 1)),
                                                     ang_to_rad(interior_angle / 2)));
        } else {
            *v = vec_rotate(*((vector_t *)list_get(star, i - 2)),
                            ang_to_rad(interior_angle));
        }
        list_add(star, v);
    }
    float red = (float)rand() / (float)(RAND_MAX);
    float green = (float)rand() / (float)(RAND_MAX);
    float blue = (float)rand() / (float)(RAND_MAX);
    body_t *new_body = body_init(star, MASS * radius, (rgb_color_t){red, green, blue});
    return new_body;
}

int main() {
    scene_t *scene = scene_init();
    sdl_init((vector_t){MIN_XY, MIN_XY}, (vector_t){MAX_X, MAX_Y});

    for (size_t i = 0; i < NUM_BODIES; i++) {
        vector_t temp_center = {(MAX_X - MAX_RADIUS) * rand() / (double)RAND_MAX,
                                (MAX_Y - MAX_RADIUS) * rand() / (double)RAND_MAX};
        double temp_r = (MAX_RADIUS * rand() / (double)RAND_MAX) + MIN_RADIUS;
        body_t *temp_body = make_body(BODY_SIDES, temp_r);
        body_set_centroid(temp_body, temp_center);
        for (size_t j = 0; j < scene_bodies(scene); j++) {
            if (i != j) {
                create_newtonian_gravity(scene, GRAV, temp_body,
                                         scene_get_body(scene, j));
            }
        }
        scene_add_body(scene, temp_body);
    }

    while (!sdl_is_done()) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    scene_free(scene);
}
