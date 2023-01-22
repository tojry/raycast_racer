#ifndef BUILDING_H
#define BUILDING_H

#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define BUILDINGS_COUNT 3

struct building_s {
    
    char* texture_name;
    int texture_width;
    int texture_height;
    float building_height;
    uint32_t* texture;
};

typedef struct building_s building_t;

void load_buildings(building_t** buildings);
building_t* load_building(char* file_name);
uint32_t* load_building_texture(char* file_name);
void free_building(building_t* building);
void free_buildings(building_t** buildings, int n);

#endif