#ifndef __FORCES_STORAGE_H__ 
#define __FORCES_STORAGE_H__

#include "scene.h"
#include "body.h"
#include "aux.h"

/**
 * A struct to hold the force_creator function, the variables that that  
 * function needs, and the free function to free those variables
 */
typedef struct force_storage force_storage_t;

/**
 * Allocates memory for a force storage
 *
 * @param forcer the force function
 * @param aux the variables to be passed into forcer
 * @param bodies the bodies associated with a force creator
 * @param freer if non-NULL, a function to free aux
 * @return a pointer to the newly allocated force_storage
 */
force_storage_t *force_storage_init(force_creator_t forcer, void *aux, list_t *bodies, free_func_t freer);

/**
 * Releases the memory allocated for the force_storage
 * 
 * @param obj a force_storage unit
 */
void force_storage_free(force_storage_t *obj);

/**
 * Gets the list of bodies
 *
 * @param obj a force_storage unit
 */
list_t *force_storage_get_bodies(force_storage_t *obj);

/**
 * Gets the force function stored
 *
 * @param obj a force_storage unit
 */
force_creator_t force_storage_get_force(force_storage_t *obj);

/**
 * Gets the pointer to the aux struct stored
 *
 * @param obj a force_storage unit
 */
void *force_storage_get_aux(force_storage_t *obj);
#endif // #ifndef __FORCES_H__
