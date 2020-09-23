#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "body.h"
#include "color.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "body_aux.h"
#include "sdl_wrapper.h"
#include "vector.h"
#include "demo_util.h"

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400

const double MIN_XY = 0;
const double MAX_X = 800;
const double MAX_Y = 400;
const double ANG_OF_CIRC = 2 * M_PI;
const double ENEMY_RADIUS = 20;
const size_t ENEMIES_IN_ROW = 8;
const double NUM_ROWS = 3;
const double VELOCITY = 100;
const double ENEMY_VELOCITY = 20;
const double LEFT_ANGLE = M_PI;
const double RIGHT_ANGLE = 0;
const double TIME_BETWEEN_BULLETS = 1;
const double PLAYER_RADIUS = 25;
const double TIME_DIVISION = 11;
const double BULLET_V = 300;
const double SCALE_MULTIPLIER = 0.5;
const double SCALE_ENEMY = 0.1;
const double MULTI_ONE = 1;

body_t *get_player(scene_t *scene){
    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_t *player = scene_get_body(scene, i);
        if(((body_aux_t*) body_get_info(player))->is_player) {
            return player;
        }
    }
    return NULL;
}

void key_movement_player(int direction, body_t *p, double angle) {
    vector_t v = vec_multiply(direction, (vector_t){VELOCITY, 0});
    body_set_velocity(p, v);
}

// creates a bullet centered at a body and adds all collisions
void bullet_collisions(body_t *body, scene_t *scene){
    body_t *bullet = make_bullet(body);
    if (((body_aux_t*) body_get_info(body))->is_player) {
        body_set_velocity(bullet, (vector_t) {0, BULLET_V}); // change back to 150
        for (size_t i = 0; i < scene_bodies(scene); i++){
            if(((body_aux_t*) body_get_info(scene_get_body(scene, i)))->is_enemy) {
                create_destructive_collision(scene, bullet, scene_get_body(scene,i));
            }
        }
    }
    else if (((body_aux_t*) body_get_info(body))->is_enemy){
        body_set_velocity(bullet, (vector_t) {0, -BULLET_V});
        create_destructive_collision(scene, bullet, scene_get_body(scene, 0));
    }
    scene_add_body(scene, bullet);
}

bool check_left_bound(body_t *player){
    vector_t left = vec_subtract(body_get_centroid(player), (vector_t) {PLAYER_RADIUS, 0});
    return left.x > MIN_XY;
}

bool check_right_bound(body_t *player){
    vector_t right = vec_add(body_get_centroid(player), (vector_t) {PLAYER_RADIUS, 0});
    return right.x < MAX_X;
}

void player_on_key(char key, key_event_type_t type, double held_time, void *scene) {
    body_t *player = get_player(scene);
    if (player != NULL){
        if (type == KEY_PRESSED){
            switch (key) {
                case (LEFT_ARROW):
                    key_movement_player(-1, player, LEFT_ANGLE);
                    break;
                case (RIGHT_ARROW):
                    key_movement_player(1, player, RIGHT_ANGLE);
                    break;
                case (' '):
                    bullet_collisions(player, scene);
                    break;
            }
            body_tick(player, held_time / TIME_DIVISION);

        }
        else {
            body_set_velocity(player, VEC_ZERO);
        }
    }
}

void generate_rows_enemies(list_t *enemy_list, scene_t *s) {
  double ENEMY_ANGLE_START = M_PI / 7;
  double ENEMY_ANGLE_END = M_PI * 6 / 7;
  double height = MAX_Y - NUM_ROWS * ENEMY_RADIUS;
  for (size_t i = 0; i < NUM_ROWS; i++){
        double temp_x = ENEMY_RADIUS;
        for (size_t j = 0; j < ENEMIES_IN_ROW; j++){
            body_t *enemy_body = make_char(ENEMY_RADIUS, ENEMY_ANGLE_START, ENEMY_ANGLE_END, SCALE_MULTIPLIER, SCALE_ENEMY);
            body_set_centroid(enemy_body, (vector_t) {temp_x, height});
            scene_add_body(s, enemy_body);
            list_add(enemy_list, enemy_body);
            temp_x += 2*ENEMY_RADIUS;
      }
      height -= 2*ENEMY_RADIUS;
  }
}

bool enemy_in_bounds(body_t *enemy) {
    vector_t enemy_pos = body_get_centroid(enemy);
    if ((enemy_pos.x + ENEMY_RADIUS > MAX_X) || (enemy_pos.y + ENEMY_RADIUS > MAX_Y))
        return false;
    if ((enemy_pos.x - ENEMY_RADIUS < MIN_XY) || (enemy_pos.y - ENEMY_RADIUS < MIN_XY))
        return false;
    return true;
}

void set_enemy_velocity(list_t *enemy_list, scene_t *s) {
    for (size_t i = 0; i < list_size(enemy_list); i++) {
        vector_t v = vec_multiply(1, (vector_t){ENEMY_VELOCITY, 0});
        body_set_velocity(list_get(enemy_list, i), v);
    }
}

void move_enemies(list_t *enemy_list, scene_t *s) {
  for (size_t i = 0; i < list_size(enemy_list); i++) {
        if(!(enemy_in_bounds(list_get(enemy_list, i)))) {
            body_t *enemy = list_get(enemy_list, i);
            vector_t v = body_get_velocity(enemy);
            v = vec_multiply(-1, (vector_t){v.x, 0});
            vector_t enemy_pos = body_get_centroid(enemy);
            body_set_centroid(enemy, (vector_t){enemy_pos.x, enemy_pos.y - 6 * ENEMY_RADIUS});
            body_set_velocity(enemy, v);
        }
  }
}

list_t *list_enemies(scene_t *scene){
    list_t *enemies = list_init(1, (free_func_t)body_free);
    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_t *enemy = scene_get_body(scene, i);
        if(((body_aux_t*) body_get_info(enemy))->is_enemy) {
            list_add(enemies, enemy);
        }
    }
    return enemies;
}

int main() {
    sdl_init((vector_t){MIN_XY, MIN_XY}, (vector_t){MAX_X, MAX_Y});
    scene_t *scene = scene_init();
    // main player
    body_t *player = make_char(PLAYER_RADIUS, 0, ANG_OF_CIRC, MULTI_ONE, MULTI_ONE);
    body_set_centroid(player, (vector_t){250, 50});
    scene_add_body(scene, player);

    list_t *enemies = list_init(1, (free_func_t)body_free);
    generate_rows_enemies(enemies, scene);
    set_enemy_velocity(enemies, scene);

    sdl_on_key(player_on_key);

    double total_time = 0;
    while (!sdl_is_done()) {
        double dt = time_since_last_tick();
        total_time += dt;
        scene_tick(scene, dt);

        if (total_time > TIME_BETWEEN_BULLETS){
            total_time = 0;
            // pick random enemy to generate bullet
            bool enemy_picked = false;

            enemies = list_enemies(scene);
            while (!enemy_picked){
                size_t num_enemies = list_size(enemies);
                size_t pick = rand() % (num_enemies - 1);
                bullet_collisions(list_get(enemies, pick), scene);
                enemy_picked = true;
            }
            move_enemies(enemies, scene);
        }

        body_t *player = get_player(scene);
        if (player != NULL){
            if (!check_left_bound(player) || !check_right_bound(player)){
               body_set_velocity(player, (vector_t){0, 0});
            }

            vector_t c = body_get_centroid(player);
            if (!check_left_bound(player)){
                body_set_centroid(player, (vector_t){MIN_XY + PLAYER_RADIUS, c.y});
            }
            if (!check_right_bound(player)){
                body_set_centroid(player, (vector_t) {MAX_X - PLAYER_RADIUS, c.y});
            }
        }
        sdl_render_scene(scene);
    }

    scene_free(scene);
}
