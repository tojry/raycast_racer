#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "vector2.h"
#include "obstacle.h"

#define ACCELERATION_RATE 0.12
#define BRAKING_FORCE 0.25
#define FRICTION 0.985
#define TURN_RATE 3.14/3.
#define MIN_SPEED 0.1
#define MAX_SPEED 25.
#define MAX_SPEED_BACKWARD 2.
#define WHEEL_SPACING 2.8
#define CAMERA_DISTANCE 3.4
#define CAR_WIDTH 1.25

struct player_s {
    
    vector2_t* position;
    float turn_angle;
    float move_velocity;
    vector2_t* cam_position;
    float cam_angle;
    bool accelerating;
    bool braking;
    bool turning_left;
    bool turning_right;
    bool on_finish_line;

    /*
    0 : Roue avant gauche
    1 : Roue avant droite
    2 : Roue arrière gauche
    3 : Roue arrière droite
    */
    vector2_t** wheels_pos;
};

typedef struct player_s player_t;

player_t* init_player(vector2_t* position);
void handle_player_inputs(player_t* player, SDL_Event event);
void update_wheels_pos(player_t* player);
void update_player_car(char* map, int map_size, player_t* player, obstacle_t** obstacles, int obstacle_count, float dt);
void update_turn_angle(player_t* player);
bool check_collision_wall(char* map, int map_size, float pos_x, float pos_y);
bool check_collision_finish(char* map, int map_size, float pos_x, float pos_y);
bool check_collision_obstacle(player_t* player, obstacle_t* obstacle);
void sat(vector2_t* axis, vector2_t** corners, float* min, float* max);
bool overlaps(float min1, float max1, float min2, float max2);
void free_player(player_t* player);

#endif