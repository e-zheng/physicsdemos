#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "color.h"
#include "list.h"
#include "polygon.h"
#include "vector.h"
#include <stdbool.h>

const double HALF = 0.5;

typedef struct body {
    list_t *shape;
    double mass;
    rgb_color_t color;
    vector_t centroid;
    vector_t velocity;
    vector_t force;
    vector_t impulse;
    double rotation;
    void *info;
    free_func_t info_freer;
    bool removed;
} body_t;

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
    void *info, free_func_t info_freer){
    body_t *new = malloc(sizeof(body_t));
    assert(new != NULL);
    assert(mass > 0);
    new->shape = shape;
    new->mass = mass;
    new->color = color;
    new->removed = false;
    // mass at rest
    new->centroid = VEC_ZERO;
    new->velocity = VEC_ZERO;
    new->force = VEC_ZERO;
    new->impulse = VEC_ZERO;
    // info stuff
    new->info = info;
    new->info_freer = info_freer;
    return new;
}

void body_free(body_t *body) {
    list_free(body->shape);
    if (body->info_freer != NULL){
        body->info_freer(body->info);
    }
    free(body);
}

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
    return body_init_with_info(shape, mass, color, NULL, NULL);
}

list_t *body_get_shape(body_t *body) {
    list_t *copy = list_init(list_size(body->shape), free);
    for (size_t i = 0; i < list_size(body->shape); i++) {
        vector_t *v = malloc(sizeof(vector_t));
        *v = *((vector_t *)list_get(body->shape, i));
        list_add(copy, v);
    }
    return copy;
}

vector_t body_get_centroid(body_t *body) {
    return body->centroid;
}

vector_t body_get_velocity(body_t *body) {
    return body->velocity;
}

double body_get_mass(body_t *body) {
    return body->mass;
}

rgb_color_t body_get_color(body_t *body) {
    return body->color;
}

vector_t body_get_impulse(body_t *body) {
    return body->impulse;
}

void *body_get_info(body_t *body){
    return body->info;
}

void body_set_info(body_t *body, void *new_info){
    body->info = new_info;
    body->info_freer = free;
}

void body_set_color(body_t *body, rgb_color_t new_color){
    body->color = new_color;
}

void body_set_centroid(body_t *body, vector_t x) {
    vector_t delta = vec_subtract(x, body->centroid);
    body->centroid = x;
    polygon_translate(body->shape, delta);
}

void body_set_velocity(body_t *body, vector_t v) {
    body->velocity = v;
}

void body_set_rotation(body_t *body, double angle) {
    polygon_rotate(body->shape, angle - body->rotation, body_get_centroid(body));
    body->rotation = angle;
}

void body_add_force(body_t *body, vector_t force) {
    body->force = vec_add(force, body->force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
    body->impulse = vec_add(impulse, body->impulse);
}

void body_tick(body_t *body, double dt) {
    vector_t v = body_get_velocity(body);
    vector_t forceimpulse = vec_multiply(dt, body->force);
    vector_t impulse = vec_add(forceimpulse, body->impulse);
    vector_t newv = vec_add(v, vec_multiply(1.0 / body->mass, impulse));
    v = vec_multiply(HALF, vec_add(v, newv));

    body_set_velocity(body, newv);
    vector_t translate = vec_multiply(dt, v);
    body_set_centroid(body, vec_add(translate, body_get_centroid(body)));
    body->impulse = VEC_ZERO;
    body->force = VEC_ZERO;
}

void body_remove(body_t *body){
    body->removed = true;
}

bool body_is_removed(body_t *body){
    return body->removed;
}
