#ifndef __COLLISION_STORAGE_H__ 
#define __COLLISION_STORAGE_H__

#include "scene.h"
#include "body.h"
#include "aux.h"
#include "forces.h"

/**
 * A struct to hold the variables that a collision handler needs
 */
typedef struct collision_storage collision_storage_t;

/**
 * Allocates memory for a collision storage
 *
 * @param aux the variables to be passed into handler 
 * @param handler the collision_handler
 * @param freer if non-NULL, a function to free the aux 
 * @return a pointer to the newly allocated force_storage
 */
collision_storage_t *collision_storage_init(void *aux, body_t *body1, body_t *body2, void* handler, free_func_t freer);

/**
 * Releases the memory allocated for the collision_storage
 * 
 * @param obj a collision_storage unit
 */
void collision_storage_free(collision_storage_t *obj);

/**
 * Gets the collision_handler function stored
 *
 * @param obj a collision_storage unit
 */
void* collision_storage_get_handler(collision_storage_t *obj);

/**
 * Gets the pointer to the aux struct stored
 *
 * @param obj a force_storage unit
 */
void *collision_storage_get_aux(collision_storage_t *obj);

/**
 * Returns the pointer to body1
 */
body_t *collision_storage_get_body1(collision_storage_t *collision_storage);
 
/**
 * Returns the pointer to body2
 */
body_t *collision_storage_get_body2(collision_storage_t *collision_storage);

void set_prev_collision(collision_storage_t *obj, bool val);

bool get_prev_collision(collision_storage_t *obj);

#endif // #ifndef __COLLISION_STORAGE_H__
