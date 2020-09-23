#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "body.h"
#include "color.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "vector.h"

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 500

const double MIN_XY = 0;
const double MAX_X = 1000;
const double MAX_Y = 500;
const double RADIUS = 12.5;
const size_t NUM_CIRCLES = 40;
const double HEIGHT = 250;
const double MASS = 5;
const double EPSILON = 0.001;
const double ANGLE_BETWEEN_POINTS = 0.05;
const double HOOK_CONST = 50.0;
const double GAMMA = 1.2;
const float COLOR_CONST = 1 / (float)4;
const double SPRING_MULTIPLER = 0.25;

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

list_t *circle_sector(size_t rad, double min_rad, double max_rad) {
    vector_t center = VEC_ZERO;
    list_t *points = list_init(1, free);
    double theta = min_rad;
    if (fabs(min_rad - 0) > EPSILON || fabs(max_rad - 2 * M_PI) > EPSILON) {
        vector_t *c = malloc(sizeof(vector_t));
        *c = center;
        list_add(points, c);
    }
    while (theta < max_rad) {
        vector_t *vertex_add = malloc(sizeof(vector_t));
        vertex_add->x = cos(theta) * rad;
        vertex_add->y = sin(theta) * rad;
        vector_t *point = malloc(sizeof(vector_t));
        *point = vec_add(center, *vertex_add);
        list_add(points, point);
        theta += ANGLE_BETWEEN_POINTS;
        free(vertex_add);
    }
    return points;
}

body_t *make_anchor(vector_t center) {
    list_t *anchor_circle = circle_sector(RADIUS, 0, 2 * M_PI);
    body_t *anchor = body_init(anchor_circle, INFINITY, (rgb_color_t){1, 1, 1});
    body_set_centroid(anchor, center);
    return anchor;
}

int main() {
    sdl_init((vector_t){MIN_XY, MIN_XY}, (vector_t){MAX_X, MAX_Y});
    scene_t *scene = scene_init();
    double curr_x = RADIUS;

    // first circle creation
    list_t *first_circle = circle_sector(RADIUS, 0, 2 * M_PI);
    body_t *first_body = body_init(first_circle, MASS, (rgb_color_t){1, 0, 0});
    body_set_centroid(first_body, (vector_t){curr_x, 0});
    body_t *anchor = make_anchor((vector_t){curr_x, HEIGHT});
    create_spring(scene, HOOK_CONST, first_body, anchor);
    scene_add_body(scene, anchor);
    scene_add_body(scene, first_body);
    create_drag(scene, GAMMA * 0, first_body);
    curr_x += 2 * RADIUS;

    for (size_t i = 1; i < NUM_CIRCLES; i++) {
        body_t *anchor = make_anchor((vector_t){curr_x, HEIGHT});
        scene_add_body(scene, anchor);

        vector_t mover_center = (vector_t){curr_x, 0};
        list_t *mover_circle = circle_sector(RADIUS, 0, 2 * M_PI);
        body_t *mover_body =
            body_init(mover_circle, MASS, change_color(scene_get_body(scene, i * 2 - 1)));
        body_set_centroid(mover_body, mover_center);

        // adding forces
        create_spring(scene, HOOK_CONST, mover_body, anchor);
        create_drag(scene, GAMMA * pow(SPRING_MULTIPLER * i, 2), mover_body);
        scene_add_body(scene, mover_body);

        curr_x += 2 * RADIUS;
    }

    while (!sdl_is_done()) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    scene_free(scene);
}
