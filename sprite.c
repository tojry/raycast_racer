#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

#include "sprite.h"
#include "SDL_utils.h"

sprite_t* init_sprite(SDL_Renderer* renderer, char* texture_file, int x, int y){

    sprite_t* sprite = (sprite_t*)malloc(sizeof(sprite_t));

    sprite->texture = load_png(texture_file, renderer);
    SDL_Rect src_pos;
    src_pos.x = 0;
    src_pos.y = 0;
    SDL_QueryTexture(sprite->texture, NULL, NULL, &src_pos.w, &src_pos.h);
    sprite->src_pos = src_pos;

    SDL_Rect dst_pos;
    dst_pos.x = x;
    dst_pos.y = y;
    dst_pos.w = src_pos.w;
    dst_pos.h = src_pos.h;

    sprite->dst_pos = dst_pos;

    return sprite;
}

void free_sprite(sprite_t* sprite){

    SDL_DestroyTexture(sprite->texture);
    free(sprite);
}