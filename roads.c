#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "roads.h"
#include "map.h"

// Initialisation d'un sommet du graphe
road_vertex_t* init_road_vertex(vector2_t* position, int edges[2], int bitmask){

    road_vertex_t* road_vertex = (road_vertex_t*)malloc(sizeof(road_vertex_t));
    road_vertex->position = position;
    road_vertex->edges[0] = edges[0];
    road_vertex->edges[1] = edges[1];
    road_vertex->bitmask = bitmask;
    return road_vertex;
}

// Construction du graphe des routes
road_vertex_t** build_roads(char* map, int map_size, float* start_x, float* start_y, float* finish_x, float* finish_y){
    
    int n = map_size * map_size;
    bool found_start = false;
    bool found_end = false;

    // Allocation mémoire des sommets
    road_vertex_t** vertices = (road_vertex_t**)malloc(sizeof(road_vertex_t*) * n);

    for(int i = 0; i < n; i++){
        vector2_t* position = init_vector2((i % map_size) * 4 + 2, (i / map_size) * 4 + 2);
        int edges[2] = {-1, -1};

        // Si on est sur un mur il n'y a pas de route, donc on passe au suivant
        if(is_wall(map, i)){ 
            vertices[i] = init_road_vertex(position, edges, -1);
            continue;
        }

        if(is_start(map, i)){
            *start_x = position->x;
            *start_y = position->y;
            found_start = true;
        }
        
        if(is_end(map, i)){
            *finish_x = position->x;
            *finish_y = position->y;
            found_end = true;
        }

        // Création des arêtes du graphe en fonction des routes voisines
        int bitmask = get_bitmask(map, map_size, i);
        int neighbor = -1;
        // Arête vers le Nord
        if(bitmask == 2 || bitmask == 6){
            if(get_neighbor(map_size, i, N, &neighbor)) edges[0] = neighbor;
        }
        // Arête vers le Sud
        else if(bitmask == 8 || bitmask == 9){
            if(get_neighbor(map_size, i, S, &neighbor)) edges[0] = neighbor;
        }
        // Arête vers l'Est
        else if(bitmask == 4 || bitmask == 12){
            if(get_neighbor(map_size, i, E, &neighbor)) edges[0] = neighbor;
        }
        // Arête vers l'Ouest
        else if(bitmask == 1 || bitmask == 3){
            if(get_neighbor(map_size, i, W, &neighbor)) edges[0] = neighbor;
        }
        // Pas de mur adjacent
        else if(bitmask == 0){
            int diag_bitmask = get_diag_bitmask(map, map_size, i);
            if(diag_bitmask == 1){
                if(get_neighbor(map_size, i, W, &neighbor)) edges[0] = neighbor;
            }
            else if(diag_bitmask == 2){
                if(get_neighbor(map_size, i, N, &neighbor)) edges[0] = neighbor;
            }
            else if(diag_bitmask == 4){
                if(get_neighbor(map_size, i, E, &neighbor)) edges[0] = neighbor;
            }
            else if(diag_bitmask == 8){
                if(get_neighbor(map_size, i, S, &neighbor)) edges[0] = neighbor;
            }
            else{
                printf("Erreur : une portion de route ne fait que 1 unite de large\n");
            }
        }
        else{
            printf("Erreur : une portion de route ne fait que 1 unite de large\n");
        }
        
        vertices[i] = init_road_vertex(position, edges, bitmask);
    }

    if(!found_start) printf("Erreur : pas de point de depart\n");
    if(!found_end) printf("Erreur : pas de point d'arrivee\n");

    return vertices;
}

// Calcul de la position des voisins
int get_bitmask(char* map, int map_size, int index){

    int bitmask = 0;
    int n;
    if(get_neighbor(map_size, index, N, &n)){
        if(is_wall(map, n)){
            bitmask += 1;
        }
    }
    if(get_neighbor(map_size, index, E, &n)){
        if(is_wall(map, n)){
            bitmask += 2;
        }
    }
    if(get_neighbor(map_size, index, S, &n)){
        if(is_wall(map, n)){
            bitmask += 4;
        }
    }
    if(get_neighbor(map_size, index, W, &n)){
        if(is_wall(map, n)){
            bitmask += 8;
        }
    }
    return bitmask;
}

// Calcul de le position des voisins en diagonale (utile pour les virages)
int get_diag_bitmask(char* map, int map_size, int index){

    int diag_bitmask = 0;
    int n;
    if(get_neighbor(map_size, index, NW, &n)){
        if(is_wall(map, n)){
            diag_bitmask += 1;
        }
    }
    if(get_neighbor(map_size, index, NE, &n)){
        if(is_wall(map, n)){
            diag_bitmask += 2;
        }
    }
    if(get_neighbor(map_size, index, SE, &n)){
        if(is_wall(map, n)){
            diag_bitmask += 4;
        }
    }
    if(get_neighbor(map_size, index, SW, &n)){
        if(is_wall(map, n)){
            diag_bitmask += 8;
        }
    }
    return diag_bitmask;
}

// Index du voisin (s'il existe)
bool get_neighbor(int map_size, int index, direction_t dir, int* neighbor){

    switch(dir){
        case N:
            *neighbor = index - map_size;
            return *neighbor >= 0;
        case S:
            *neighbor = index + map_size;
            return *neighbor < map_size * map_size;
        case E:
            *neighbor = index + 1;
            return *neighbor / map_size == index / map_size;
        case W:
            *neighbor = index - 1;
            return *neighbor / map_size == index / map_size && *neighbor >= 0;
        case NW:
            *neighbor = index - map_size - 1;
            return *neighbor >= 0;
        case NE:
            *neighbor = index - map_size + 1;
            return *neighbor >= 0;
        case SE:
            *neighbor = index + map_size + 1;
            return *neighbor < map_size * map_size;
        case SW:
            *neighbor = index + map_size - 1;
            return *neighbor < map_size * map_size;
    }
}

// Affichage du plan des routes (en mode debug)
void render_roads_map(uint32_t* texture, int width, road_vertex_t** roads, int map_size){

    int n = map_size * map_size;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < 2; j++){
            if(roads[i]->bitmask == -1 || roads[i]->edges[j] == -1)
                continue;
            else{
                draw_edge(texture, width, roads[i]->position, roads[roads[i]->edges[j]]->position, map_size);
                draw_vertex(texture, width, roads[i]->position, map_size);
            }
        }
    }
}

// Tracé d'une arête (en mode debug)
void draw_edge(uint32_t* texture, int width, vector2_t* a, vector2_t* b, int map_size){

    int start_x = fmin(a->x, b->x) * width / (4 * map_size);
    int end_x = fmax(a->x, b->x) * width / (4 * map_size);
    int start_y = fmin(a->y, b->y) * width / (4 * map_size);
    int end_y = fmax(a->y, b->y) * width / (4 * map_size);

    if(start_x == end_x){
        for(int y = start_y; y < end_y; y++){
            texture[start_x + y * width] = 0x00000000;
        }
    }else{
        for(int x = start_x; x < end_x; x++){
            texture[x + start_y * width] = 0x00000000;
        }
    }
}

// Tracé d'un sommet (en mode debug)
void draw_vertex(uint32_t* texture, int width, vector2_t* a, int map_size){

    int cx = a->x * width / (4 * map_size);
    int cy = a->y * width / (4 * map_size);

    for(int x = cx-2; x <= cx+2; x++){
        for(int y = cy-2; y <= cy+2; y++){
            if(x < 0 || y < 0 || x >= width || y >= width) continue;
            texture[x + y * width] = 0x00000000;
        }
    }
}

// Libération de la mémoire
void free_roads(road_vertex_t** roads, int map_size){

    int n = map_size * map_size;
    for(int i = 0; i < n; i++){
        free_vector2(roads[i]->position);
        free(roads[i]);
    }
    free(roads);
}