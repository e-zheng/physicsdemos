#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "body.h"
#include "list.h"
#include "aux.h"
#include "scene.h"
#include "force_storage.h"
#include <stdio.h>

const size_t BODIES_TO_ALLOCATE = 30;
const size_t CREATORS_TO_ALLOCATE = 100;

typedef struct scene{
    list_t* bodies;
    size_t num_bodies;
    list_t* force_creators;
}scene_t;

typedef void (*force_creator_t)(void *aux);

scene_t *scene_init(void){
    scene_t *sc = malloc(sizeof(scene_t));
    assert(sc != NULL);
    sc->bodies = list_init(BODIES_TO_ALLOCATE, (free_func_t) body_free);
    sc->num_bodies = 0;
    sc->force_creators = list_init(CREATORS_TO_ALLOCATE, (free_func_t)force_storage_free);
    return sc;
}

void scene_free(scene_t *scene){
    list_free(scene->bodies);
    list_free(scene->force_creators);
    free(scene);
}

size_t scene_bodies(scene_t *scene){
    return scene->num_bodies;
}

body_t *scene_get_body(scene_t *scene, size_t index){
    assert(index < scene->num_bodies);
    return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body){
    list_add(scene->bodies, body);
    scene->num_bodies++;
}

void scene_remove_body(scene_t *scene, size_t index){
    assert(index < scene_bodies(scene));
    body_remove(list_get(scene->bodies, index));
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
    void *aux, list_t *bodies, free_func_t freer){
    force_storage_t *storage = force_storage_init(forcer, aux, bodies, freer);
    list_add(scene->force_creators, storage);
    
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux, free_func_t freer){
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, aux_get_body1(aux));
    if (aux_get_body2(aux) != NULL){
        list_add(bodies, aux_get_body2(aux));
    }
    scene_add_bodies_force_creator(scene, forcer, aux, bodies, freer);
}

void scene_tick(scene_t *scene, double dt){
    // does all forces
    for(size_t f = 0; f < list_size(scene->force_creators); f++){
        force_storage_t *storage = list_get(scene->force_creators, f);

        force_creator_t func = force_storage_get_force(storage);
        func(force_storage_get_aux(storage));
    }

    // removes force creators
    for(size_t f = 0; f < list_size(scene->force_creators); f++){
        force_storage_t *storage = list_get(scene->force_creators, f);
        list_t *bodies = force_storage_get_bodies(storage);
        
        for(size_t b = 0; b < list_size(bodies); b++){
            if (body_is_removed(list_get(bodies, b))){
                list_remove(scene->force_creators, f);
                force_storage_free(storage);
                f--;
                break;
            }
        }
    }

    for(size_t i = 0; i < scene_bodies(scene); i++){
        if (body_is_removed(list_get(scene->bodies, i))){
            void *removed = list_remove(scene->bodies, i);
            body_free(removed);
            scene->num_bodies--;
            i--;
        }
        else{
            body_tick(scene_get_body(scene, i), dt);
    
        }
    }
}
