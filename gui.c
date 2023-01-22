#include <math.h>

#include "game.h"
#include "sprite.h"
#include "gui.h"
#include "SDL_utils.h"

menu_t* init_menu(SDL_Renderer* renderer, int width, int height, int map_count){

    menu_t* menu = (menu_t*)malloc(sizeof(menu_t));

    menu->background = init_sprite(renderer, "menu/background.png", 0, 0);
    menu->settings_title = init_sprite(renderer, "menu/title_settings.png", -60, 30);
    menu->render_dist_background = init_sprite(renderer, "menu/background_render_dist.png", 0, height/2 - 90);
    menu->select_title = init_sprite(renderer, "menu/title_select.png", 0, 30);
    menu->loading_title = init_sprite(renderer, "menu/title_loading.png", 0, 30);
    menu->finish_title = init_sprite(renderer, "menu/title_finish.png", 0, 30);
    menu->finish_time_background = init_sprite(renderer, "menu/background_empty.png", 0, 120);
    menu->best_time_background = init_sprite(renderer, "menu/background_empty.png", 0, 190);
    menu->new_best_title = init_sprite(renderer, "menu/title_new_best.png", 0, 260);
    menu->speed_background = init_sprite(renderer, "menu/background_speed.png", width - 150, height - 36);
    menu->time_background = init_sprite(renderer, "menu/background_time.png", -170, 6);

    menu->button_play = init_button(renderer, 150, height/2 - 100, 300, 60, "menu/button_play.png", 0);
    menu->button_settings = init_button(renderer, 150 , height/2 - 20, 300, 60, "menu/button_options.png", 1);
    menu->button_quit = init_button(renderer, 150, height/2 + 60, 300, 60, "menu/button_quit.png", 2);

    menu->button_increase_render_dist =  init_button(renderer, width/2 + 60, height/2, 60, 60, "menu/right_arrow.png", 3);
    menu->button_decrease_render_dist =  init_button(renderer, width/2 - 60, height/2, 60, 60, "menu/left_arrow.png", 4);
    menu->button_back = init_button(renderer, 150, height - 90, 300, 60, "menu/button_back.png", 5);

    menu->button_reset = init_button(renderer, 225, height/2 + 70, 450, 60, "menu/button_reset.png", 6);

    menu->buttons_map = (button_t**)malloc(sizeof(button_t*) * map_count);
    for(int i = 0; i < map_count; i++){
        menu->buttons_map[i] = init_button(renderer, 225, 140 + i * 65, 450, 60, "menu/background_empty.png", LAST_BUTTON_ID + i);
    }

    return menu;
}

sprite_t** init_car_sprites(SDL_Renderer* renderer){

    sprite_t** sprites = (sprite_t**)malloc(sizeof(sprite_t*) * 6);

    sprites[0] = init_sprite(renderer, "car/car.png", 0, 0);
    sprites[1] = init_sprite(renderer, "car/car_braking.png", 0, 0);
    sprites[2] = init_sprite(renderer, "car/car_turn_left.png", 0, 0);
    sprites[3] = init_sprite(renderer, "car/car_turn_right.png", 0, 0);
    sprites[4] = init_sprite(renderer, "car/car_turn_left_braking.png", 0, 0);
    sprites[5] = init_sprite(renderer, "car/car_turn_right_braking.png", 0, 0);

    return sprites;
}

button_t* init_button(SDL_Renderer* renderer, int x, int y, int width, int height, char* sprite_path, int id){

    sprite_t* sprite = init_sprite(renderer, sprite_path, x - width/2, y - height/2);
    
    button_t* button = (button_t*)malloc(sizeof(button_t));
    button->center_x = x;
    button->center_y = y;
    button->width = width;
    button->height = height;
    button->sprite = sprite;
    button->id = id;
    
    return button;
}

// Gestion des événements des menus (boutons)
void handle_menu_events(menu_t* menu, SDL_Event event, game_t* game){

    if(game->game_state == MENU){
        handle_button_events(menu->button_play, event, game);
        handle_button_events(menu->button_settings, event, game);
        handle_button_events(menu->button_quit, event, game);
    }
    else if(game->game_state == SETTINGS){
        handle_button_events(menu->button_back, event, game);
        handle_button_events(menu->button_increase_render_dist, event, game);
        handle_button_events(menu->button_decrease_render_dist, event, game);
        handle_button_events(menu->button_reset, event, game);
    }
    else if(game->game_state == SELECT_MAP){
        handle_button_events(menu->button_back, event, game);
        for(int i = 0; i < game->map_count; i++){
            handle_button_events(menu->buttons_map[i], event, game);
        }
    }
    else if(game->game_state == FINISH_RACE){
        handle_button_events(menu->button_back, event, game);
    }
}

// Détection du clic sur un bouton
void handle_button_events(button_t* button, SDL_Event event, game_t* game){

    int x = 0;
    int y = 0;

    x = event.motion.x;
    y = event.motion.y;

    if(is_mouse_over(button, x, y)){
        if(event.type == SDL_MOUSEBUTTONDOWN){
            if(event.button.button == SDL_BUTTON_LEFT){
                click(game, button->id);
            }
        }
    }
}

// Détection du survol d'un bouton par la souris
bool is_mouse_over(button_t* button, int mouse_x, int mouse_y){

    int half_width = button->width >> 1;
    int half_height = button->height >> 1;
    return mouse_x > button->center_x - half_width && mouse_x < button->center_x + half_width
        && mouse_y > button->center_y - half_height && mouse_y < button->center_y + half_height;
}

// Clic sur un bouton
void click(game_t* game, int id){

    if(id == 0){// Jouer
        game->game_state = SELECT_MAP;
    }
    else if(id == 1){// Options
        game->game_state = SETTINGS;
    }
    else if(id == 2){// Quitter
        game->end = true;
    }
    else if(id == 3){// Augmenter la distance d'affichage
        update_render_distance(game, 5);
    }
    else if(id == 4){// Diminuer la distance d'affichage
        update_render_distance(game, -5);
    }
    else if(id == 5){// Retour au menu
        game->game_state = MENU;
        if(!load(game)){
            save(game);
        }
    }
    else if(id == 6){// Réinitialisation de la sauvegarde
        game = init_game();
        save(game);
    }
    else if(id >= LAST_BUTTON_ID){// Chargement d'une course
        game->game_state = LOADING;
        game->current_map = id - LAST_BUTTON_ID;
    }
}

void free_menu(menu_t* menu, int map_count){

    free_sprite(menu->background);
    free_sprite(menu->settings_title);
    free_sprite(menu->render_dist_background);
    free_sprite(menu->select_title);
    free_sprite(menu->loading_title);
    free_sprite(menu->finish_title);
    free_sprite(menu->finish_time_background);
    free_sprite(menu->best_time_background);
    free_sprite(menu->new_best_title);
    free_sprite(menu->speed_background);
    free_sprite(menu->time_background);

    free_button(menu->button_play);
    free_button(menu->button_settings);
    free_button(menu->button_quit);
    free_button(menu->button_back);
    free_button(menu->button_increase_render_dist);
    free_button(menu->button_decrease_render_dist);
    free_button(menu->button_reset);
    for(int i = 0; i < map_count; i++){
        free_button(menu->buttons_map[i]);
    }
    free(menu->buttons_map);
    free(menu);
}

void free_car_sprites(sprite_t** sprites){

    for(int i = 0; i < 6; i++){
        free_sprite(sprites[i]);
    }
    free(sprites);
}

void free_button(button_t* button){

    free_sprite(button->sprite);
    free(button);
}

// Affichage du texte
void draw_text(SDL_Renderer* renderer, TTF_Font* font, SDL_Color* color, int x, int y, char text[100], anchor_type_e anchor){

    SDL_Texture* texture = load_text(text, renderer, font, *color);
    int w,h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect rect = {x, y, w, h};
    
    // Décalage du point d'ancrage
    if(anchor == CENTER){
        rect.x -= w/2;
        rect.y -= h/2;
    }
    else if(anchor == TOP_RIGHT){
        rect.x -= w;
    }
    else if(anchor == BOTTOM_LEFT){
        rect.y -= h;
    }
    else if(anchor == BOTTOM_RIGHT){
        rect.x -= w;
        rect.y -= h;
    }

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
}

// Affichage de l'interface en jeu
void draw_game_gui(SDL_Renderer* renderer, TTF_Font* font, int width, int height, game_t* game, menu_t* menu, sprite_t** car_sprites){

    // Affichage du chronomètre
    SDL_RenderCopy(renderer, menu->time_background->texture, NULL, &menu->time_background->dst_pos);
    char timer_text[100] = "000.000 s";
    sprintf(timer_text, "%d.%d s", (int)game->current_time, (int)((game->current_time - (int)game->current_time) * 1000));
    
    // Affichage de la vitesse
    SDL_RenderCopy(renderer, menu->speed_background->texture, NULL, &menu->speed_background->dst_pos);
    char speed_text[100] = "000 km/h";
    sprintf(speed_text, "%d km/h", (int)fabs(game->player->move_velocity * 4));
    SDL_Color gray = {45,52,54,255};

    draw_text(renderer, font, &gray, 15, 10, timer_text, TOP_LEFT);
    draw_text(renderer, font, &gray, width - 15, height-10, speed_text, BOTTOM_RIGHT);

    // Affichage du sprite de la voiture
    if((game->player->braking && game->player->move_velocity >= 0) || (game->player->accelerating && game->player->move_velocity < 0)){
        if(game->player->turning_left){
            // Braking and turning left
            SDL_RenderCopy(renderer, car_sprites[4]->texture, &car_sprites[4]->src_pos, &car_sprites[4]->dst_pos);
        }
        else if(game->player->turning_right){
            // Braking and turning right
            SDL_RenderCopy(renderer, car_sprites[5]->texture, &car_sprites[5]->src_pos, &car_sprites[5]->dst_pos);
        } else {
            // Braking
            SDL_RenderCopy(renderer, car_sprites[1]->texture, &car_sprites[1]->src_pos, &car_sprites[1]->dst_pos);
        }
    } else if(fabs(game->player->move_velocity) > 0 && (game->player->turning_left || game->player->turning_right)){
        if(game->player->turning_left){
            // Turning left
            SDL_RenderCopy(renderer, car_sprites[2]->texture, &car_sprites[2]->src_pos, &car_sprites[2]->dst_pos);
        }
        if(game->player->turning_right){
            // Turning right
            SDL_RenderCopy(renderer, car_sprites[3]->texture, &car_sprites[3]->src_pos, &car_sprites[3]->dst_pos);
        }
    } else {
        // Default
        SDL_RenderCopy(renderer, car_sprites[0]->texture, &car_sprites[0]->src_pos, &car_sprites[0]->dst_pos);
    }
}

// Affichage du menu principal
void draw_main_menu_gui(SDL_Renderer* renderer, menu_t* menu){

    SDL_RenderCopy(renderer, menu->background->texture, NULL, &menu->background->dst_pos);

    SDL_RenderCopy(renderer, menu->button_play->sprite->texture, NULL, &menu->button_play->sprite->dst_pos);
    SDL_RenderCopy(renderer, menu->button_settings->sprite->texture, NULL, &menu->button_settings->sprite->dst_pos);
    SDL_RenderCopy(renderer, menu->button_quit->sprite->texture, NULL, &menu->button_quit->sprite->dst_pos);
}

// Affichage du menu d'options
void draw_settings_gui(SDL_Renderer* renderer, TTF_Font* font, menu_t* menu, int width, int height, int render_dist){

    // Sprites
    SDL_RenderCopy(renderer, menu->background->texture, NULL, &menu->background->dst_pos);
    SDL_RenderCopy(renderer, menu->settings_title->texture, NULL, &menu->settings_title->dst_pos);
    SDL_RenderCopy(renderer, menu->render_dist_background->texture, NULL, &menu->render_dist_background->dst_pos);

    // Boutons
    SDL_RenderCopy(renderer, menu->button_back->sprite->texture, NULL, &menu->button_back->sprite->dst_pos);
    SDL_RenderCopy(renderer, menu->button_increase_render_dist->sprite->texture, NULL, &menu->button_increase_render_dist->sprite->dst_pos);
    SDL_RenderCopy(renderer, menu->button_decrease_render_dist->sprite->texture, NULL, &menu->button_decrease_render_dist->sprite->dst_pos);
    SDL_RenderCopy(renderer, menu->button_reset->sprite->texture, NULL, &menu->button_reset->sprite->dst_pos);

    // Textes
    char render_dist_text[3] = "000";
    sprintf(render_dist_text, "%d", render_dist);
    SDL_Color white = {255,255,255,255};

    draw_text(renderer, font, &white, width/2, height/2, render_dist_text, CENTER);
}

// Affichage du menu de sélection de la course
void draw_select_map_gui(SDL_Renderer* renderer, TTF_Font* font, menu_t* menu, game_t* game){

    SDL_RenderCopy(renderer, menu->background->texture, NULL, &menu->background->dst_pos);
    SDL_RenderCopy(renderer, menu->select_title->texture, NULL, &menu->select_title->dst_pos);

    SDL_Color white = {255,255,255,255};
    // Affichage des boutons des courses et des meilleurs temps
    for(int i = 0; i < game->map_count; i++){
        SDL_RenderCopy(renderer, menu->buttons_map[i]->sprite->texture, NULL, &menu->buttons_map[i]->sprite->dst_pos);
        
        char best_time_text[100] = "Course 0 - 000.000 s";
        if(game->best_times[i] > 0){
            sprintf(best_time_text, "Course %d - %d.%d s", i+1, (int)game->best_times[i], (int)((game->best_times[i] - (int)game->best_times[i]) * 1000));
        }else{
            sprintf(best_time_text, "Course %d", i+1);
        }
        int coord_y = menu->buttons_map[i]->sprite->dst_pos.y + 10;
        draw_text(renderer, font, &white, 20, coord_y, best_time_text, TOP_LEFT);
    }

    SDL_RenderCopy(renderer, menu->button_back->sprite->texture, NULL, &menu->button_back->sprite->dst_pos);
}

// Affichage du menu de fin de course
void draw_finish_race_gui(SDL_Renderer* renderer, TTF_Font* font, menu_t* menu, game_t* game){

    // Sprites
    SDL_RenderCopy(renderer, menu->background->texture, NULL, &menu->background->dst_pos);
    SDL_RenderCopy(renderer, menu->finish_title->texture, NULL, &menu->finish_title->dst_pos);
    SDL_RenderCopy(renderer, menu->finish_time_background->texture, NULL, &menu->finish_time_background->dst_pos);
    SDL_RenderCopy(renderer, menu->best_time_background->texture, NULL, &menu->best_time_background->dst_pos);
    if(game->new_best_time){
        SDL_RenderCopy(renderer, menu->new_best_title->texture, NULL, &menu->new_best_title->dst_pos);
    }

    // Bouton
    SDL_RenderCopy(renderer, menu->button_back->sprite->texture, NULL, &menu->button_back->sprite->dst_pos);

    // Textes
    char time_text[100] = "Temps : 000.000 s";
    sprintf(time_text, "Temps : %d.%d s", (int)game->current_time, (int)((game->current_time - (int)game->current_time) * 1000));
    char best_time_text[100] = "Record : 000.000 s";
    sprintf(best_time_text, "Record : %d.%d s", (int)game->best_times[game->current_map], (int)((game->best_times[game->current_map] - (int)game->best_times[game->current_map]) * 1000));
    
    SDL_Color white = {255,255,255,255};
    draw_text(renderer, font, &white, 30, 170, time_text, BOTTOM_LEFT);
    draw_text(renderer, font, &white, 30, 240, best_time_text, BOTTOM_LEFT);
}

void draw_loading_gui(SDL_Renderer* renderer, menu_t* menu){

    SDL_RenderCopy(renderer, menu->background->texture, NULL, &menu->background->dst_pos);
    SDL_RenderCopy(renderer, menu->loading_title->texture, NULL, &menu->loading_title->dst_pos);
}


