#ifndef ROAD_H
#define ROAD_H

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vector2.h"
#include "map.h"

// Structure de graphe (liste d'adjacence)
typedef struct {

    // Position du sommet
    vector2_t* position;
    // Index des sommets reliés
    int edges[2];
    // Position des murs voisins
    int bitmask;

} road_vertex_t;

road_vertex_t* init_road_vertex(vector2_t* position, int edges[2], int bitmask);
road_vertex_t** build_roads(char* map, int map_size, float* start_x, float* start_y, float* finish_x, float* finish_y);
int get_bitmask(char* map, int map_size, int index);
int get_diag_bitmask(char* map, int map_size, int index);
bool get_neighbor(int map_size, int index, direction_t dir, int* neighbor);
void render_roads_map(uint32_t* texture, int width, road_vertex_t** roads, int map_size);
void draw_edge(uint32_t* texture, int width, vector2_t* a, vector2_t* b, int map_size);
void draw_vertex(uint32_t* texture, int width, vector2_t* a, int map_size);
void free_roads(road_vertex_t** roads, int map_size);

#endif