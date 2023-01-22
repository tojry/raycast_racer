#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "game.h"
#include "SDL_utils.h"
#include "sprite.h"
#include "renderer.h"
#include "player.h"
#include "vector2.h"
#include "map.h"
#include "building.h"
#include "roads.h"
#include "obstacle.h"
#include "gui.h"

int main(int argc, char *argv[]){

    (void)argc;
    (void)(argv);

    bool debug = false;

    srand(time(NULL));

    // Initialisation des données du jeu
    game_t* game = init_game();
    if(!load(game)){
        save(game);
    }

    SDL_Window* window;
    SDL_Event event;
    SDL_Renderer* renderer;
    
    // Initialisation de la SDL
    if(init_SDL(&renderer, &window, SCREEN_WIDTH, SCREEN_HEIGHT) != 0){
        return EXIT_FAILURE;
    }

    // Initialisation des polices
    TTF_Font *font24, *font43, *font58;
    init_font(&font24, "font.ttf", 24);
    init_font(&font43, "font.ttf", 43);
    init_font(&font58, "font.ttf", 58);

    // Texture qui contient le rendu 3D
    SDL_Texture *main_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Tableau de pixel de la vue 3D
    uint32_t* pixels = init_texture(SCREEN_WIDTH, SCREEN_HEIGHT, 0xffffffff);
    
    // Initialisation des bâtiments
    building_t* buildings[BUILDINGS_COUNT];
    load_buildings(buildings);

    // Initialisation des sprites de la voiture
    sprite_t** car_sprites = init_car_sprites(renderer);
    
    // Initialisation des éléments des menus
    menu_t* menu = init_menu(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, game->map_count);

    int last_ticks = 0;

    while(!game->end){

        // Mise à jour du delta time
        float delta_time = (SDL_GetTicks() - last_ticks) / 1000.;
        last_ticks = SDL_GetTicks();
        if(delta_time == 0)
            delta_time = 1.;
        if(delta_time > 1){
            delta_time = 0.01;
        }
        
        // Initialisation de la course si c'est une nouvelle
        if(game->game_state == LOADING && game->start_new_race){
            init_race(game);
        }

        // Mise à jour du chronomètre
        update_timer(game, delta_time);

        // Gestion des événements
        while(SDL_PollEvent(&event) != 0){

            if(event.type == SDL_QUIT){
                game->end = true;
                break;
            }

            if(game->game_state == GAME){
                handle_player_inputs(game->player, event);

                // Retour au menu si on appuie sur ECHAP
                if(event.type == SDL_KEYDOWN && event.key.repeat == 0){
                    if(event.key.keysym.sym == SDLK_ESCAPE){
                        clear_race_data(game);
                        game->game_state = MENU;
                    }
                }
            } else {
                handle_menu_events(menu, event, game);
            }
        }

        SDL_RenderClear(renderer);

        if(game->game_state == GAME){
            
            // Mise à jour des éléments du jeu
            update_player_car(game->map, game->map_size, game->player, game->obstacles, game->obstacle_count, delta_time);
            update_obstacles(game->obstacles, game->obstacle_count, game->roads, delta_time);

            if(debug){
                // Affichage du graphe des routes en mode debug
                render_roads_map(pixels, SCREEN_WIDTH, game->roads, game->map_size);
            } else {
                // Affichage du rendu 3D
                render(pixels, buildings, game->obstacles, game->map, game->player, SCREEN_WIDTH, SCREEN_HEIGHT, game->map_size, game->obstacle_count, game->render_dist);
            }

            SDL_UpdateTexture(main_texture, NULL, (void*)pixels, SCREEN_WIDTH * 4);
            SDL_RenderCopy(renderer, main_texture, NULL, NULL);
            
            // Affichage de l'interface
            if(!debug){
                draw_game_gui(renderer, font24, SCREEN_WIDTH, SCREEN_HEIGHT, game, menu, car_sprites);
            }

            // Détection de fin de la course
            if(check_collision_finish(game->map, game->map_size, game->player->wheels_pos[0]->x, game->player->wheels_pos[0]->y)){
                reach_finish_line(game);
            }
        }
        // Affichage des éléments d'interface correspondants
        else if(game->game_state == MENU){
            draw_main_menu_gui(renderer, menu);
        }
        else if(game->game_state == SETTINGS){
            draw_settings_gui(renderer, font43, menu, SCREEN_WIDTH, SCREEN_HEIGHT, game->render_dist);
        }
        else if(game->game_state == SELECT_MAP){
            draw_select_map_gui(renderer, font43, menu, game);
        }
        else if(game->game_state == LOADING){
            draw_loading_gui(renderer, menu);
        }
        else if(game->game_state == FINISH_RACE){
            draw_finish_race_gui(renderer, font43, menu, game);
        }
        SDL_RenderPresent(renderer);

        SDL_Delay(2);
    }

    // Libération de la mémoire
    free_car_sprites(car_sprites);
    free(pixels);
    free_buildings(buildings, BUILDINGS_COUNT);
    free_menu(menu, game->map_count);
    free_game(game);

    TTF_CloseFont(font24);
    TTF_CloseFont(font43);
    TTF_CloseFont(font58);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}