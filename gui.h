#ifndef GUI_H
#define GUI_H

#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "game.h"

#define LAST_BUTTON_ID 7

typedef enum{
    CENTER,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
} anchor_type_e;

typedef struct {

    int center_x;
    int center_y;
    int width;
    int height;
    sprite_t* sprite;
    int id;

} button_t;

typedef struct {

    sprite_t* background;
    button_t* button_play;
    button_t* button_settings;
    button_t* button_quit;
    button_t* button_increase_render_dist;
    button_t* button_decrease_render_dist;
    button_t* button_back;
    button_t* button_reset;
    button_t** buttons_map;

    sprite_t* settings_title;
    sprite_t* render_dist_background;
    sprite_t* select_title;
    sprite_t* loading_title;
    sprite_t* finish_title;
    sprite_t* finish_time_background;
    sprite_t* best_time_background;
    sprite_t* new_best_title;
    sprite_t* speed_background;
    sprite_t* time_background;

} menu_t;

menu_t* init_menu(SDL_Renderer* renderer, int width, int height, int map_count);
button_t* init_button(SDL_Renderer* renderer, int x, int y, int width, int height, char* sprite_path, int id /*int (*on_click)()*/);
sprite_t** init_car_sprites(SDL_Renderer* renderer);
void handle_menu_events(menu_t* menu, SDL_Event event, game_t* game);
void handle_button_events(button_t* button, SDL_Event event, game_t* game);
bool is_mouse_over(button_t* button, int mouse_x, int mouse_y);
void click(game_t* game, int id);
void free_menu(menu_t* menu, int map_count);
void free_button(button_t* button);
void free_car_sprites(sprite_t** sprites);
void draw_text(SDL_Renderer* renderer, TTF_Font* font, SDL_Color* color, int x, int y, char text[100], anchor_type_e anchor);
void draw_game_gui(SDL_Renderer* renderer, TTF_Font* font, int width, int height, game_t* game, menu_t* menu, sprite_t** car_sprites);
void draw_main_menu_gui(SDL_Renderer* renderer, menu_t* menu);
void draw_settings_gui(SDL_Renderer* renderer, TTF_Font* font, menu_t* menu, int width, int height, int render_dist);
void draw_select_map_gui(SDL_Renderer* renderer, TTF_Font* font, menu_t* menu, game_t* game);
void draw_finish_race_gui(SDL_Renderer* renderer, TTF_Font* font, menu_t* menu, game_t* game);
void draw_loading_gui(SDL_Renderer* renderer, menu_t* menu);

#endif