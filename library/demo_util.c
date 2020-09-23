#include "demo_util.h"

#include "vector.h"
#include "list.h"
#include "body.h"
#include "scene.h"
#include "color.h"
#include "polygon.h"
#include "body_aux.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

const double EP = 0.001;
const double ANGLE_BTW_POINTS = 1.0;
const double ANG_OF_CIRCLE = 2 * M_PI;
const double ANGLE_B_POINTS = 0.05;
const double ANGLE_OF_CIRC = 2 * M_PI;
const double SCALE = 0.5;
const rgb_color_t PLAYER_COLOR = (rgb_color_t){0.41, 0.41, 0.41};
const double PLAYER_HEIGHT = 10;
const double PLAYER_WIDTH = 20;
const double PLAYER_MASS = 20;

const double BULLET_WIDTH = 1.5;
const double BULLET_HEIGHT = 5;
const double BULLET_MASS = 2;

const rgb_color_t ENEMY_COLOR= (rgb_color_t) {0.8, 0.8, 0.8};
const double ENEMY_MASS = 20;


body_t *make_char(size_t rad, double min_rad, double max_rad, double scale1, double scale2){
    vector_t center = VEC_ZERO;
    list_t *points = list_init(1, free);
    double theta = min_rad;
    vector_t *c = malloc(sizeof(vector_t));
    *c = center;
    list_add(points, c);
    while (theta < max_rad) {
        vector_t *vertex_add = malloc(sizeof(vector_t));
        vertex_add->x = cos(theta) * rad;
        vertex_add->y = scale1 * sin(theta) * rad;
        vector_t *point = malloc(sizeof(vector_t));
        *point = vec_add(center, *vertex_add);
        list_add(points, point);
        theta += ANGLE_BTW_POINTS * scale2;
        free(vertex_add);
    }
    body_aux_t *temp_aux = malloc(sizeof(body_aux_t));
    if (max_rad == ANG_OF_CIRCLE){
        *temp_aux = (body_aux_t){true, false};
        return body_init_with_info(points, PLAYER_MASS, PLAYER_COLOR, temp_aux, free);
    }
    *temp_aux = (body_aux_t){false, true};
    return body_init_with_info(points, ENEMY_MASS, ENEMY_COLOR, temp_aux, free);
}
    

body_t *make_bullet(body_t *maker){
    vector_t center = polygon_centroid(body_get_shape(maker));
    list_t *points = list_init(1, free);
    // add points individually
    vector_t *point1 = malloc(sizeof(vector_t));
    *point1 = (vector_t){center.x + BULLET_WIDTH, center.y + BULLET_HEIGHT};
    vector_t *point2 = malloc(sizeof(vector_t));
    *point2 = (vector_t){center.x + BULLET_WIDTH, center.y - BULLET_HEIGHT};
    vector_t *point3 = malloc(sizeof(vector_t));
    *point3 = (vector_t){center.x - BULLET_WIDTH, center.y - BULLET_HEIGHT};
    vector_t *point4 = malloc(sizeof(vector_t));
    *point4 = (vector_t){center.x - BULLET_WIDTH, center.y + BULLET_HEIGHT};
    list_add(points, point1);
    list_add(points, point2);
    list_add(points, point3);
    list_add(points, point4);
    body_aux_t *maker_aux = body_get_info(maker);
    bool player = maker_aux ->is_player;
    body_aux_t *bullet_aux = malloc(sizeof(body_aux_t));
    *bullet_aux = (body_aux_t){false, false};
    if (player == true){
        return body_init_with_info(points, BULLET_MASS, PLAYER_COLOR, bullet_aux, free);
    }
    else {
        return body_init_with_info(points, BULLET_MASS, ENEMY_COLOR, bullet_aux, free);
    }
}
