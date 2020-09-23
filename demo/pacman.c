#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "body.h"
#include "color.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "vector.h"

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 500

const double MOUTH_ANGLE = M_PI / 5;
const double ANGLE_BETWEEN_POINTS = 0.05;
const double CIRCLE_POINTS = 30;
const double ANGLE_OF_CIRCLE = 2 * M_PI;
const int PELLET_RAD = 5;
const double MULTIPLIER = 1.5;
const int PAC_RAD = 50;
const double EPSILON = 0.001;
const int INIT_V = 50;
const double PELLET_MASS = 100.0;
const int NUM_PELLETS = 20;
const int TICK = 2;
const int CENTER_X = 100;
const int CENTER_Y = 100;

const double TOP_ANGLE = M_PI / 2;
const double LEFT_ANGLE = M_PI;
const double RIGHT_ANGLE = 0;
const double BOTTOM_ANGLE = 3 * M_PI / 2;

list_t *circle_sector(vector_t center, size_t rad, double min_rad, double max_rad) {
    list_t *points = list_init(1, free);
    double theta = min_rad;
    if (fabs(min_rad - 0) > EPSILON || fabs(max_rad - ANGLE_OF_CIRCLE) > EPSILON) {
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

void gen_pellet(scene_t *scene) {
    vector_t center = (vector_t){rand() % (WINDOW_WIDTH - PELLET_RAD),
                                 rand() % (WINDOW_HEIGHT - PELLET_RAD)};
    body_t *pellet = body_init(circle_sector(center, PELLET_RAD, 0, ANGLE_OF_CIRCLE),
                               PELLET_MASS, (rgb_color_t){1, 1, 0});
    scene_add_body(scene, pellet);
}

void key_movement_lr(double dt, body_t *pacman, double angle) {
    body_set_rotation(pacman, angle);
    vector_t v =
        vec_add(body_get_velocity(pacman), vec_multiply(-dt, (vector_t){MULTIPLIER, 0}));
    body_set_velocity(pacman, v);
}

void key_movement_ud(double dt, body_t *pacman, double angle) {
    body_set_rotation(pacman, angle);
    vector_t v =
        vec_add(body_get_velocity(pacman), vec_multiply(-dt, (vector_t){0, MULTIPLIER}));
    body_set_velocity(pacman, v);
}

void pacman_wrap(body_t *pacman) {
    vector_t new_centroid = polygon_centroid(body_get_shape(pacman));
    if (new_centroid.x > WINDOW_WIDTH + PAC_RAD) {
        body_set_centroid(pacman, (vector_t){-1 * PAC_RAD - new_centroid.x, 0});
    }
    if (new_centroid.x < 0 - PAC_RAD) {
        body_set_centroid(pacman, (vector_t){WINDOW_WIDTH + PAC_RAD - new_centroid.x, 0});
    }
    if (new_centroid.y > WINDOW_HEIGHT + PAC_RAD) {
        body_set_centroid(pacman, (vector_t){0, -1 * PAC_RAD - new_centroid.y});
    }
    if (new_centroid.y < 0 - PAC_RAD) {
        body_set_centroid(pacman,
                          (vector_t){0, WINDOW_HEIGHT + PAC_RAD - new_centroid.y});
    }
}

void on_key(char key, key_event_type_t type, double held_time, void *pacman) {
    if (type == KEY_PRESSED) {
        switch (key) {
            case (LEFT_ARROW):
                key_movement_lr(held_time, pacman, LEFT_ANGLE);
                break;
            case (UP_ARROW):
                key_movement_ud(-held_time, pacman, TOP_ANGLE);
                break;
            case (RIGHT_ARROW):
                key_movement_lr(-held_time, pacman, RIGHT_ANGLE);
                break;
            case (DOWN_ARROW):
                key_movement_ud(held_time, pacman, BOTTOM_ANGLE);
                break;
        }
        body_tick(pacman, held_time);
    } else {
        body_set_velocity(pacman, VEC_ZERO);
    }
}

void remove_pellets(scene_t *scene, body_t *pac) {
    list_t *pac_points = body_get_shape(pac);
    vector_t pac_center = body_get_centroid(pac);
    vector_t *pt_mid = list_get(pac_points, 0);
    vector_t *pt_top = list_get(pac_points, 1);
    vector_t *pt_last = list_get(pac_points, list_size(pac_points) - 1);
    size_t i = 1;
    size_t num_bodies = scene_bodies(scene);
    while (i < num_bodies) {
        body_t *temp_body = scene_get_body(scene, i);
        vector_t pellet_center = body_get_centroid(temp_body);
        double distance = sqrt(pow(pac_center.x - pellet_center.x, 2) +
                               pow(pac_center.y - pellet_center.y, 2));
        if (distance < PAC_RAD) {
            // uses the distance between point (x, y) and line formula:
            // distance = abs(Ax + By + C)/sqrt(A^2 + B^2)
            double slope1 = (pt_mid->y - pt_top->y) / (pt_mid->x - pt_top->x);
            double c1 = slope1 * -1 * pt_top->x + pt_top->y;
            double distance1 = (fabs(slope1 * pellet_center.x - pellet_center.y + c1) /
                                sqrt(1 + pow(slope1, 2)));
            double slope2 = (pt_mid->y - pt_last->y) / (pt_mid->x - pt_last->x);
            double c2 = slope2 * -1 * pt_last->x + pt_last->y;
            double distance2 = (fabs(slope2 * pellet_center.x - pellet_center.y + c2) /
                                sqrt(1 + pow(slope2, 2)));
            if (distance1 < PELLET_RAD || distance2 < PELLET_RAD) {
                scene_remove_body(scene, i);
                num_bodies--;
            } else {
                i++;
            }
        } else {
            i++;
        }
    }
}

int main() {
    vector_t bound = {WINDOW_WIDTH, WINDOW_HEIGHT};
    sdl_init(VEC_ZERO, bound);
    scene_t *scene = scene_init();

    vector_t center = (vector_t){CENTER_X, CENTER_Y};
    body_t *pacman = body_init(
        circle_sector(center, PAC_RAD, MOUTH_ANGLE, ANGLE_OF_CIRCLE - MOUTH_ANGLE),
        PELLET_MASS, (rgb_color_t){1, 1, 0});
    scene_add_body(scene, pacman);

    for (size_t i = 0; i < NUM_PELLETS; i++) {
        gen_pellet(scene);
    }

    double time = 0.0;
    sdl_on_key(on_key);

    while (!sdl_is_done()) {
        double dt = time_since_last_tick();
        time += dt;
        remove_pellets(scene, pacman);
        if (time > TICK) {
            gen_pellet(scene);
            time = 0.0;
        }
        pacman_wrap(pacman);
        sdl_render_scene(scene);
    }
    scene_free(scene);
}
