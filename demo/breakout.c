#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "body_aux.h"
#include "forces.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "collision.h"
#include "demo_breakout_util.h"

#include "body.h"
#include <string.h>
#include <stdbool.h>

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400

const double MIN_XY = 0;
const double MAX_X = 800;
const double MAX_Y = 400;
const double WHITE_SPACE = 7;
const double WALL_SPACING = 4;
const double RECT_HEIGHT = 20;
const double ENEMY_NUM_ROW = 3;
const double ENEMY_NUM_COL = 10;
const double RECT_WIDTH = (MAX_X - (ENEMY_NUM_COL+2)*WHITE_SPACE)/ENEMY_NUM_COL;
const double CIRCLE_RADIUS = 5;
const vector_t CIRCLE_INIT_VELOCITY = {MAX_X/4, MAX_Y/2};
const vector_t PLAYER_INIT = {MAX_X/2, MIN_XY + RECT_HEIGHT/2};
const vector_t CIRCLE_INIT = {MAX_X/2, MIN_XY + RECT_HEIGHT + CIRCLE_RADIUS};
const rgb_color_t PLAYER_COLOR_BREAKOUT =  (rgb_color_t) {1, 0, 0};
const double VELOCITY = 100;
const double LEFT_ANGLE = M_PI;
const double RIGHT_ANGLE = 0;
const double TIME_DIVISION = 11;
const double ELASTICITY = 1;

body_t *get_player(scene_t *scene){
    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_t *player = scene_get_body(scene, i);
        if(((body_aux_t*) body_get_info(player))->is_player) {
            return player;
        }
    }
    return NULL;
}

body_t *get_circle(scene_t *scene){
    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_t *circle = scene_get_body(scene, i);
        if(((body_aux_t*) body_get_info(circle))->is_circle) {
            return circle;
        }
    }
    return NULL;
}

rgb_color_t change_color(rgb_color_t color) {
    double change = 0.25;
    float r = color.r;
    float g = color.g;
    float b = color.b;
    if (r >= 1 && g < 1 && b <= 0) {
        g += change;
    }
    if (g >= 1 && r > 0 && b <= 0) {
        r -= change;
    }
    if (g >= 1 && b < 1 && r <= 0) {
        b += change;
    }
    if (b >= 1 && g > 0 && r <= 0) {
        g -= change;
    }
    if (b >= 1 && r < 1 && g <= 0) {
        r += change;
    }
    if (r >= 1 && b > 0 && g <= 0) {
        b -= change;
    }
    return (rgb_color_t){r, g, b};
}

void generate_rainbow_enemies(list_t *enemy_list, scene_t *s, body_t *bouncer){
    double height = MAX_Y - RECT_HEIGHT/2 - WHITE_SPACE;
    for (int i = 0; i < ENEMY_NUM_ROW; i++){
        double width = MIN_XY + RECT_WIDTH/2 + WHITE_SPACE;
        rgb_color_t color = (rgb_color_t) PLAYER_COLOR_BREAKOUT;
        for (int j = 0; j < ENEMY_NUM_COL; j++){
            body_aux_t *enemy_aux = malloc(sizeof(body_aux_t));
            *enemy_aux = (body_aux_t){false, true, false, 2};
            body_t *enemy_body = make_rect(RECT_WIDTH, RECT_HEIGHT, color, 1);
            body_set_info(enemy_body, enemy_aux);
            body_set_centroid(enemy_body, (vector_t) {width, height});
            scene_add_body(s, enemy_body);
            create_physics_collision(s, ELASTICITY, bouncer, enemy_body);
            list_add(enemy_list, enemy_body);
            width += RECT_WIDTH + WHITE_SPACE;
            color = change_color(color);
        }
        height -= RECT_HEIGHT + WHITE_SPACE;
    }
}

void key_movement_player(int direction, body_t *p, double angle) {
    vector_t v = vec_multiply(direction, (vector_t){VELOCITY, 0});
    body_set_velocity(p, v);
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
            }
            body_tick(player, held_time / TIME_DIVISION);
        }
        else {
            body_set_velocity(player, VEC_ZERO);
        }
    }
}

bool check_left_bound(body_t *player){
    vector_t left = vec_subtract(polygon_centroid(body_get_shape(player)), (vector_t) {20, 0});
    return left.x > MIN_XY;
}
bool check_right_bound(body_t *player){
    vector_t right = vec_add(polygon_centroid(body_get_shape(player)), (vector_t) {20, 0});
    return right.x < MAX_X;
}

// keeps player from moving offscreen
void player_update(body_t *player){
    if (player != NULL){
        if (!check_left_bound(player) || !check_right_bound(player)){
            body_set_velocity(player, (vector_t){0, 0});
            if (!check_left_bound(player)){
                body_set_centroid(player, (vector_t){RECT_WIDTH/2, RECT_HEIGHT/2});
            }
            else {
                body_set_centroid(player, (vector_t) {MAX_X - RECT_WIDTH/2, RECT_HEIGHT/2});
            }
        }
    }
}

void reset_scene(scene_t *scene){
    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_remove(scene_get_body(scene, i));
    }
    body_aux_t *player_aux = malloc(sizeof(body_aux_t));
    *player_aux = (body_aux_t){true, false, false, -1};
    body_t *player = make_rect(RECT_WIDTH, RECT_HEIGHT, PLAYER_COLOR_BREAKOUT, 0);
    body_set_centroid(player, PLAYER_INIT);
    body_set_info(player, player_aux);
    scene_add_body(scene, player);

    body_aux_t *circle_aux = malloc(sizeof(body_aux_t));
    *circle_aux = (body_aux_t){false, false, true, -1};
    body_t *circle = circle_sector_breakout(CIRCLE_RADIUS);
    body_set_centroid(circle, CIRCLE_INIT);
    scene_add_body(scene, circle);
    body_set_info(circle, circle_aux);
    body_set_velocity(circle, CIRCLE_INIT_VELOCITY);
    create_physics_collision(scene, ELASTICITY, circle, player);
    
    list_t *enemies = list_init(1, (free_func_t)body_free);
    generate_rainbow_enemies(enemies, scene, circle);

    body_aux_t *props = malloc(sizeof(body_aux_t));
    *props = (body_aux_t){false, false, false, -1};
    body_t *left_wall =
        make_rect(1.5 * WALL_SPACING, MAX_Y, PLAYER_COLOR_BREAKOUT, 2);
    body_set_centroid(left_wall, (vector_t) {MIN_XY - WALL_SPACING, MAX_Y / 2});
    body_set_info(left_wall, props);
    scene_add_body(scene, left_wall);
    create_physics_collision(scene, ELASTICITY, circle, left_wall);

    body_aux_t *props2 = malloc(sizeof(body_aux_t));
    *props2 = (body_aux_t){false, false, false, -1};
    body_t *right_wall =
        make_rect(1.5 * WALL_SPACING, MAX_Y, PLAYER_COLOR_BREAKOUT, 2);
    body_set_centroid(right_wall, (vector_t) {MAX_X + WALL_SPACING, MAX_Y / 2});
    body_set_info(right_wall, props2);
    scene_add_body(scene, right_wall);
    create_physics_collision(scene, ELASTICITY, circle, right_wall);

    body_aux_t *props3 = malloc(sizeof(body_aux_t));
    *props3 = (body_aux_t){false, false, false, -1};
    body_t *top_wall =
        make_rect(MAX_X, 1.5 * WALL_SPACING, PLAYER_COLOR_BREAKOUT, 2);
    body_set_centroid(top_wall, (vector_t) {MAX_X / 2, MAX_Y + WALL_SPACING});
    body_set_info(top_wall, props3);
    scene_add_body(scene, top_wall);
    create_physics_collision(scene, ELASTICITY, circle, top_wall);
}

int main(){
    sdl_init((vector_t){MIN_XY, MIN_XY}, (vector_t){MAX_X, MAX_Y});
    scene_t *scene = scene_init();
    double total_time = 0.0;
    reset_scene(scene);

    sdl_on_key(player_on_key);
    while (!sdl_is_done()){
        body_t *circle = get_circle(scene);
        body_t *player = get_player(scene);
        player_update(player);
        double dt = time_since_last_tick();
        total_time += dt;
        if(body_get_centroid(circle).y < CIRCLE_RADIUS){
            reset_scene(scene);
        }
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
}
