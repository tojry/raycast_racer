#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

struct sprite_s {
    
    SDL_Texture* texture;
    SDL_Rect src_pos;
    SDL_Rect dst_pos;

};

typedef struct sprite_s sprite_t;

sprite_t* init_sprite(SDL_Renderer* renderer, char* texture_file, int x, int y);
void free_sprite(sprite_t* sprite);

#endif