#include <stdlib.h>
#include <math.h>

#include "vector2.h"

vector2_t* init_vector2(float x, float y){

    vector2_t* v = (vector2_t*)malloc(sizeof(vector2_t));
    v->x = x;
    v->y = y;

    return v;
}

float distance(vector2_t* a, vector2_t* b){

    return sqrt((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
}

// Produit scalaire
float dot(vector2_t* a, vector2_t* b){

    return a->x * b->x + a->y * b->y;
}

void free_vector2(vector2_t* v){

    free(v);
}