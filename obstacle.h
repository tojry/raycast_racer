#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "sprite.h"
#include "vector2.h"
#include "roads.h"

#define OBS_CAR_SPEED 3.
#define OBS_CAR_WIDTH 1.7
#define OBS_CAR_LENGTH 4.
#define OBS_SPRITE_SIZE 256

typedef struct {

    vector2_t* position;
    float direction;
    float length;
    float width;
    uint32_t* texture;
    int next_vertex;
    float height_scale;
    vector2_t** corners;
    float player_dst;
    bool moving;
    int spritesheet_width;
    int spritesheet_height;

} obstacle_t;

obstacle_t* init_obstacle(vector2_t* position, char* sprite_name, int vertex, bool moving);
uint32_t* load_obstacle_texture(char* file_name, int* width, int* height);
void update_obstacles(obstacle_t** obstacles, int obstacle_count, road_vertex_t** roads, float delta_time);
void update_corners(obstacle_t* obstacle);
void move_towards_next_vertex(road_vertex_t** roads, obstacle_t* obstacle, float dt);
bool reached_next_vertex(road_vertex_t** roads, obstacle_t* obstacle);
void free_obstacle(obstacle_t* obstacle);

#endif