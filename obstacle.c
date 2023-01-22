#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "SDL_utils.h"
#include "obstacle.h"

// Initialisation d'un obstacle
obstacle_t* init_obstacle(vector2_t* position, char* sprite_name, int vertex, bool moving){

    obstacle_t* obstacle = (obstacle_t*)malloc(sizeof(obstacle_t));
    obstacle->position = position;
    obstacle->direction = 0;
    obstacle->length = OBS_CAR_LENGTH;
    obstacle->width = OBS_CAR_WIDTH;
    obstacle->texture = load_obstacle_texture(sprite_name, &(obstacle->spritesheet_width), &(obstacle->spritesheet_height));
    obstacle->next_vertex = vertex;
    obstacle->height_scale = 4;
    obstacle->moving = moving;

    obstacle->corners = (vector2_t**)malloc(sizeof(vector2_t*) * 4);
    for(int i = 0; i < 4; i++){
        obstacle->corners[i] = init_vector2(0,0);
    }
    update_corners(obstacle);

    return obstacle;
}

// Chargement du sprite d'un obstacle
uint32_t* load_obstacle_texture(char* file_name, int* width, int* height){

    SDL_Surface* surface = load_png_surface(file_name);
    
    int tex_w = surface->w;
    int tex_h = surface->h;

    uint32_t* img = (uint32_t*)malloc(sizeof(uint32_t) * tex_w * tex_h);
    
    for(int i = 0; i < tex_w; i++){
        for(int j = 0; j < tex_h; j++){
           
            uint8_t r = ((uint8_t*)(surface->pixels))[(i+j*tex_w)*4+3];
            uint8_t g = ((uint8_t*)(surface->pixels))[(i+j*tex_w)*4+2];
            uint8_t b = ((uint8_t*)(surface->pixels))[(i+j*tex_w)*4+1];
            uint8_t a = ((uint8_t*)(surface->pixels))[(i+j*tex_w)*4+0];
            
            img[i+j*tex_w] = convert_color(r, g, b, a);
            
        }
    }
    SDL_FreeSurface(surface);

    *width = tex_w / OBS_SPRITE_SIZE;
    *height = tex_h / OBS_SPRITE_SIZE;
    return img;
}

// Mise à jour des obstacles
void update_obstacles(obstacle_t** obstacles, int obstacle_count, road_vertex_t** roads, float delta_time){

    for(int i = 0; i < obstacle_count; i++){
        if(obstacles[i]->moving){
            move_towards_next_vertex(roads, obstacles[i], delta_time);
        }
    }
}

// Mise à jour des coins de l'obstacle
void update_corners(obstacle_t* obstacle){

    float dir = obstacle->direction;
    float center_x = obstacle->position->x;
    float center_y = obstacle->position->y;
    float half_width = obstacle->width * 0.5;
    float half_length = obstacle->length * 0.5;

    obstacle->corners[0]->x = center_x + (half_length * cos(dir) - half_width * sin(dir));
    obstacle->corners[0]->y = center_y + (half_length * sin(dir) + half_width * cos(dir));

    obstacle->corners[1]->x = center_x + (half_length * cos(dir) + half_width * sin(dir));
    obstacle->corners[1]->y = center_y + (half_length * sin(dir) - half_width * cos(dir));

    obstacle->corners[2]->x = center_x - (half_length * cos(dir) - half_width * sin(dir));
    obstacle->corners[2]->y = center_y - (half_length * sin(dir) + half_width * cos(dir));

    obstacle->corners[3]->x = center_x - (half_length * cos(dir) + half_width * sin(dir));
    obstacle->corners[3]->y = center_y - (half_length * sin(dir) - half_width * cos(dir));
}

// Déplacement vers la position du prochain sommet du graphe
void move_towards_next_vertex(road_vertex_t** roads, obstacle_t* obstacle, float dt){

    vector2_t* next_position = roads[obstacle->next_vertex]->position;

    float dir = atan2(obstacle->position->y - next_position->y, obstacle->position->x - next_position->x);

    obstacle->direction = dir;

    float move_x = -cos(dir) * OBS_CAR_SPEED * dt;
    float move_y = -sin(dir) * OBS_CAR_SPEED * dt;

    obstacle->position->x += move_x;
    obstacle->position->y += move_y;

    update_corners(obstacle);
    
    // Si le sommet est atteint, on en choisit un nouveau
    if(reached_next_vertex(roads, obstacle)){
        int r = rand() % 2;
        if(roads[obstacle->next_vertex]->edges[1] == -1){
            r = 0;
        }
        obstacle->next_vertex = roads[obstacle->next_vertex]->edges[r];
    }
}

// Vérifie si le sommet est atteint
bool reached_next_vertex(road_vertex_t** roads, obstacle_t* obstacle){

    vector2_t* next_position = roads[obstacle->next_vertex]->position;
    return distance(obstacle->position, next_position) < 0.2;
}

// Libération de la mémoire
void free_obstacle(obstacle_t* obstacle){

    free(obstacle->texture);
    free_vector2(obstacle->position);
    for(int i = 0; i < 4; i++){
        free_vector2(obstacle->corners[i]);
    }
    free(obstacle->corners);
    free(obstacle);
}