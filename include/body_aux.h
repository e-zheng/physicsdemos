#ifndef __BODY_AUX_H__
#define __BODY_AUX_H__

#include <stdbool.h>

typedef struct {
    bool is_player;
    bool is_enemy;
    bool is_circle;
    // if body is not to be removed (e.g. walls or players, set health < 0)
    // if body is to be removed, set health > 0
    // remove body if health == 0
    int health;
} body_aux_t;

extern const body_aux_t PROP;
extern const body_aux_t SPACE_ENEMY;
extern const body_aux_t SPACE_PLAYER;
extern const body_aux_t BRICK;

#endif // #ifndef __BODY_AUX_H__
