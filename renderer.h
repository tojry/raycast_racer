#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "player.h"
#include "building.h"
#include "obstacle.h"

#define PI 3.141592568
#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512
#define SKY_COLOR 0xbce7ffff
#define GROUND_COLOR 0x737373ff
#define FOV 1.0467 // PI / 3
#define MAX_BUILDING_HEIGHT 2.0
#define OBSTACLE_TEXTURE_SIZE 256

uint32_t* init_texture(int width, int height, uint32_t color);
void draw_column(uint32_t* texture, int width, int height, building_t* building, int tex_coord, int x, int min_y, int max_y, int max_y_ground, float current_height);
void clear_texture(uint32_t* texture, int width, int height);

void render(uint32_t* texture, building_t** buildings, obstacle_t** obstacles, char* map, player_t* player, int width, int height, int map_size, int obstacle_count, int render_dst);
void draw_obstacle(uint32_t* texture, int width, int height, float depth_buffer[width], obstacle_t* sprite, player_t* player);

#endif