#include "collision_storage.h"
#include "forces.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "list.h"
#include "scene.h"

typedef struct collision_storage {
    aux_t *aux;
    collision_handler_t handler;
    free_func_t free_func;
    body_t *body1;
    body_t *body2;
    bool prev_collision;
} collision_storage_t;

collision_storage_t *collision_storage_init(void *aux, body_t *body1, body_t *body2, void* handler, free_func_t freer){
    collision_storage_t *new = malloc(sizeof(collision_storage_t));
    assert(new != NULL);
    new->handler = (collision_handler_t) handler;
    new->aux = aux;
    new->free_func = freer;
    // NULL if aux holds ??
    new->body1 = body1;
    new->body2 = body2;
    new->prev_collision = false;
    return new;
}

void collision_storage_free(collision_storage_t *obj) {
    if (obj->free_func != NULL){
        obj->free_func(obj->aux);
    }
    free(obj);
}   

void* collision_storage_get_handler(collision_storage_t *obj) {
    return obj->handler;
}

void *collision_storage_get_aux(collision_storage_t *obj) {
    return obj->aux;
}

body_t *collision_storage_get_body1(collision_storage_t *obj){
    return obj->body1;
}
 
body_t *collision_storage_get_body2(collision_storage_t *obj){
    return obj->body2;
}

void set_prev_collision(collision_storage_t *obj, bool val){
    obj->prev_collision = val;
}

bool get_prev_collision(collision_storage_t *obj){
    return obj->prev_collision;
}
