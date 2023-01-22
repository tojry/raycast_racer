#ifndef VECTOR2_H
#define VECTOR2_H

struct vector2_s {
    
    float x;
    float y;
};

typedef struct vector2_s vector2_t;

vector2_t* init_vector2(float x, float y);
float distance(vector2_t* a, vector2_t* b);
float dot(vector2_t* a, vector2_t* b);
void free_vector2(vector2_t* v);

#endif