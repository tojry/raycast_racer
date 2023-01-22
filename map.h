#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

#include "vector2.h"

#define MAP_SCALE 4.

typedef enum { N,E,S,W,NW,NE,SE,SW } direction_t;

char* load_map(char* file_name, int* size, int* obstacle_count);
bool is_wall(char* map, int index);
bool is_start(char* map, int index);
bool is_end(char* map, int index);

#endif
