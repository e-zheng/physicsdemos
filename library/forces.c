#include "forces.h"
#include "body.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "aux.h"
#include "collision.h"
#include "body_aux.h"
#include "color.h"
#include "collision_storage.h"

const double TOO_CLOSE_GRAV = 75.0;
const float COLOR_UPDATE = 0.1;

void gravity_force(aux_t *aux){
    double G = aux_get_constant(aux);
    body_t *body1 = aux_get_body1(aux);
    body_t *body2 = aux_get_body2(aux);
    double m1 = body_get_mass(body1);
    double m2 = body_get_mass(body2);

    vector_t r2minus1= vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    double r12 = sqrt(vec_dot(r2minus1, r2minus1));
    vector_t r = vec_multiply(1.0 / r12, r2minus1);
    if (r12 > TOO_CLOSE_GRAV){
        // define how close we want bodies to be for force to be ignored
        vector_t F21 = vec_multiply((-G * m1 * m2 / (r12 * r12)), r);
        body_add_force(body2, F21);
        body_add_force(body1, vec_negate(F21));
    }

}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2){
    aux_t *grav_vars = aux_init(G, body1, body2);
    scene_add_force_creator(scene, (force_creator_t) gravity_force, (void*) grav_vars, (free_func_t) aux_free);
}

void spring_force(aux_t *aux){
    double k = aux_get_constant(aux);
    body_t *body1 = aux_get_body1(aux);
    body_t *body2 = aux_get_body2(aux);
    vector_t Fx = vec_multiply(k, vec_subtract(body_get_centroid(body2), body_get_centroid(body1)));
    body_add_force(body1, Fx);
    body_add_force(body2, vec_negate(Fx));
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2){
    aux_t *spring_vars = aux_init(k, body1, body2);
    scene_add_force_creator(scene, (force_creator_t) spring_force, (void*) spring_vars, (free_func_t) aux_free);
}

void drag_force(aux_t *aux){
    double gamma = aux_get_constant(aux);
    body_t *body1 = aux_get_body1(aux);
    vector_t v = body_get_velocity(body1);
    body_add_force(body1, vec_multiply(-gamma, v));
}

void create_drag(scene_t *scene, double gamma, body_t *body){
    aux_t *drag_vars = aux_init(gamma, body, NULL);
    scene_add_force_creator(scene, (force_creator_t) drag_force, (void*) drag_vars, (free_func_t) aux_free);
}

//collision_handler_t
void destructive_collision(body_t *body1, body_t *body2, vector_t axis, void *aux){
    body_remove(body1);
    body_remove(body2);
}

void health_diminish(body_t *body){
    body_aux_t *aux = (body_aux_t*) body_get_info(body);

    if(aux->is_enemy){
        aux->health--;
        // update color
        if (aux->health > 0){
            body_set_color(body, lighter_color(body_get_color(body)));
        }

        if (aux->health == 0){
            body_remove(body);
        }
    }
}

void physics_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    health_diminish(body1);
    health_diminish(body2);

    double elasticity_val = aux_get_constant(aux);
    double player_dot_product = vec_dot(body_get_velocity(body1), axis);
    double enemy_dot_product = vec_dot(body_get_velocity(body2), axis);

    if (body_get_mass(body2) == INFINITY) {
        double impulse = body_get_mass(body1) * (1 + elasticity_val) * (enemy_dot_product - player_dot_product);
        body_add_impulse(body1, vec_multiply(impulse, axis));
    }
    else {
        double reduced_mass = body_get_mass(body1) * body_get_mass(body2) / (body_get_mass(body1) + body_get_mass(body2));
        double impulse = reduced_mass * (1 + elasticity_val) * (enemy_dot_product - player_dot_product);
        body_add_impulse(body1, vec_multiply(impulse, axis));
        body_add_impulse(body2, vec_multiply(-1 *impulse, axis));
    }
}

void collision(void *collision_storage){
    body_t *body1 = collision_storage_get_body1(collision_storage);
    body_t *body2 = collision_storage_get_body2(collision_storage);
    
    collision_info_t *collision = find_collision(body_get_shape(body1), body_get_shape(body2));
    if (collision->collided){
        if (!get_prev_collision(collision_storage)){
            collision_handler_t handler = (collision_handler_t) collision_storage_get_handler(collision_storage);
            handler(body1, body2,collision->axis, collision_storage_get_aux(collision_storage));
            set_prev_collision(collision_storage, true); 
        }
        else{
            set_prev_collision(collision_storage, false);
        }
    }
}

// general function -- handlers are functions
void create_collision(scene_t *scene, body_t *body1, body_t *body2, collision_handler_t handler, void *aux, free_func_t freer) {
    list_t *bodies = list_init(2, (free_func_t) body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);

    collision_storage_t *col_stor = collision_storage_init(aux, body1, body2, handler, freer);
    scene_add_bodies_force_creator(scene, (force_creator_t) collision, (void*) col_stor, bodies, (free_func_t) collision_storage_free);

}

// calls create_collision with destructive_collision as handler
void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2){
    aux_t *destroy_vars = aux_init(0.0, body1, body2);
    create_collision(scene, body1, body2, (collision_handler_t) destructive_collision,
        (void*) destroy_vars, (free_func_t) aux_free);
}

void create_physics_collision(scene_t *scene, double elasticity, body_t *body1, body_t *body2) {
    aux_t *physics_vars = aux_init(elasticity, body1, body2);
    create_collision(scene, body1, body2, (collision_handler_t) physics_collision,
        (void*) physics_vars, (free_func_t) aux_free);
}
