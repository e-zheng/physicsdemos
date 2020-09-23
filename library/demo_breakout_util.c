#include "demo_breakout_util.h"

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

const rgb_color_t CIRCLE_COLOR = (rgb_color_t) {1, 0, 0};
const double CIRCLE_MASS = 1;
const double RECT_MASS = INFINITY;
const double MAX_RAD = 2 * M_PI;
const double BETWEEN_POINTS = 0.05;
const int INIT_HEALTH = 2;

body_t *make_rect(double width, double height, rgb_color_t color, int player){
    vector_t centroid = VEC_ZERO;
    vector_t half_shape = {width/2, height/2};
    list_t *rect = list_init(4, free);
    vector_t *v = malloc(sizeof(*v));
    for (int i = 0; i < 2; i++){
      v = malloc(sizeof(*v));
      *v = vec_add(centroid, half_shape);
      list_add(rect, v);
      half_shape.x  *= -1;
      v = malloc(sizeof(*v));
      *v = vec_subtract(centroid, half_shape);
      list_add(rect, v);
      half_shape.y  *= -1;
    }
    double mass = RECT_MASS;
    body_aux_t *temp_aux = malloc(sizeof(body_aux_t));
    if (player == 0){
      *temp_aux = (body_aux_t){true, false, false};
    }else if (player == 1){
      *temp_aux = (body_aux_t){false, true, false};
    }else{
       *temp_aux = (body_aux_t) {true, true, true};
       mass = INFINITY;
    }
    //free half_shape
    return body_init_with_info(rect, mass, color, temp_aux, free);
}

body_t *circle_sector_breakout(double radius){
    vector_t center = VEC_ZERO;
    list_t *circle = list_init(1, free);
    double theta = 0;
    while (theta < MAX_RAD){
        vector_t *vertex_add = malloc(sizeof(vector_t));
        vertex_add->x = cos(theta) * radius;
        vertex_add->y = sin(theta) * radius;
        vector_t *point = malloc(sizeof(vector_t));
        *point = vec_add(center, *vertex_add);
        list_add(circle, point);
        theta += BETWEEN_POINTS;
        free(vertex_add);
    }
    body_aux_t *temp_aux = malloc(sizeof(body_aux_t));
    *temp_aux = (body_aux_t){false, false, true};
    return body_init_with_info(circle, CIRCLE_MASS, CIRCLE_COLOR, temp_aux, free);
}
