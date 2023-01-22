#ifndef SORT_H
#define SORT_H

#include "obstacle.h"

void swap(obstacle_t** tab, int i, int j);
int split(obstacle_t** tab, int lower, int upper);
void sort(obstacle_t** tab, int lower, int upper);

#endif