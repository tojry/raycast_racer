#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

int init_SDL(SDL_Renderer** renderer, SDL_Window** window, int width, int height);
void init_font(TTF_Font** font, char* font_name, int size);
SDL_Texture* load_text(const char* message, SDL_Renderer* renderer, TTF_Font *font, SDL_Color color);
SDL_Texture* load_png(const char* nomfichier, SDL_Renderer* renderer);
SDL_Surface* load_png_surface(char* file_name);
uint32_t convert_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void read_color(uint32_t* color, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);

#endif