#include "force_storage.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "list.h"
#include "scene.h"

typedef struct force_storage {
    force_creator_t force;
    aux_t *aux;
    list_t *bodies;
    free_func_t free_func;
} force_storage_t;

force_storage_t *force_storage_init(force_creator_t forcer, void *aux, list_t *bodies, free_func_t freer) {
    force_storage_t *new = malloc(sizeof(force_storage_t));
    assert(new != NULL);
    new->force = forcer;
    new->aux = aux;
    assert(bodies != NULL);
    new->bodies = bodies;
    new->free_func = freer;
    return new;
}

void force_storage_free(force_storage_t *obj) {
    if (obj->free_func != NULL){
        obj->free_func(obj->aux);
    }
    free(obj);
}

list_t *force_storage_get_bodies(force_storage_t *obj){
    return obj->bodies;
}

force_creator_t force_storage_get_force(force_storage_t *obj) {
    return obj->force;
}

void *force_storage_get_aux(force_storage_t *obj) {
    return obj->aux;
}
