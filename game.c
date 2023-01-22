#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "game.h"
#include "map.h"
#include "vector2.h"

// Initialisation du jeu
game_t* init_game(){

    game_t* game = (game_t*)malloc((sizeof(game_t)));
    game->game_state = MENU;
    game->end = false;
    game->render_dist = 70;

    // Comptage du nombre de fichiers de map
    DIR * maps_dir;
    struct dirent * entry;
    int map_count = 0;
    maps_dir = opendir("maps");
    while ((entry = readdir(maps_dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            map_count++;
        }
    }
    closedir(maps_dir);
    if(map_count > 4){
        printf("Erreur : trop de fichiers de course");
    }
    game->map_count = map_count;

    // Chargement des meilleurs scores
    for(int i = 0; i < map_count; i++){
        game->best_times[i] = -1;
    }
    game->current_map = 0;
    game->start_new_race = true;
    game->new_best_time = false;

    return game;
}

// Initialisation de la course
void init_race(game_t* game){

    // Initialisation des batiments
    int size = 0;
    int obstacle_count = 0;
    char map_path[20];
    sprintf(map_path, "maps/map%d.txt", game->current_map);
    game->map = load_map(map_path, &size, &obstacle_count);
    game->map_size = size;
    game->obstacle_count = obstacle_count;

    // Initialisation du graphe des routes
    float start_x, start_y, finish_x, finish_y;
    game->roads = build_roads(game->map, size, &start_x, &start_y, &finish_x, &finish_y);

    // Initialisation du joueur
    game->player = init_player(init_vector2(start_x, start_y));
    
    // Initialisation des obstacles
    game->obstacles[0] = init_obstacle(init_vector2(finish_x,finish_y), "finish_line.png", size*size, false);
    for(int i = 1; i < game->obstacle_count; i++){
        int index = 0;
        do{
            index = rand() % size;
            index *= (rand() % size);
        }while(game->roads[index]->bitmask == -1);

        vector2_t* pos = init_vector2((index % size) * 4 + 2, (index / size) * 4 + 2);
        game->obstacles[i] = init_obstacle(pos, "obstacle/spritesheet.png", index, true);
    }

    game->current_time = 0;
    game->start_new_race = false;
    game->game_state = GAME;
}

// Modification de la distance d'affichage
void update_render_distance(game_t* game, int val){

    game->render_dist += val;
    if(game->render_dist < 30) game->render_dist = 30;
    if(game->render_dist > 100) game->render_dist = 100;
    save(game);
}

// Lorsque la ligne d'arrivée est atteinte
void reach_finish_line(game_t* game){

    // On remplace le meilleur temps s'il est battu
    if(game->current_time < game->best_times[game->current_map] || game->best_times[game->current_map] < 0){
        game->best_times[game->current_map] = game->current_time;
        game->new_best_time = true;
    } else {
        game->new_best_time = false;
    }
    save(game);
    game->game_state = FINISH_RACE;
    clear_race_data(game);
}

// Libération de la mémoire des données de la course
void clear_race_data(game_t* game){

    for(int i = 0; i < game->obstacle_count; i++){
        free_obstacle(game->obstacles[i]);
    }
    free_roads(game->roads, game->map_size);
    free_player(game->player);
    free(game->map);
    game->start_new_race = true;
}

// Lecture du fichier de sauvegarde
bool load(game_t* game){

    FILE *load_file;

    load_file = fopen("save.txt","r");
    if(load_file == NULL){
        
        return false;
    }

    // Distance d'affichage
    char str_render_dist[5];
    fgets(str_render_dist, 5, load_file);
    int render_dist;
    sscanf(str_render_dist, "%d", &render_dist);

    // Lecture des meilleurs scores de chaque map
    float best_times[game->map_count];
    for(int i = 0; i < game->map_count; i++){
        char str_best_time[30];
        fgets(str_best_time, 30, load_file);
        float best_time;
        sscanf(str_best_time, "%f", &best_time);
        best_times[i] = best_time;
        if(i > 0 && best_times[i-1] == best_time){
            best_times[i] = -1;
        }
    }

    game->render_dist = render_dist;
    for(int i = 0; i < game->map_count; i++){
        game->best_times[i] = best_times[i];
    }
    
    fclose(load_file);

    return true;
}

// Ecriture du fichier de sauvegarde
void save(game_t* game){

    FILE *save_file;

    save_file = fopen("save.txt","w");

    if(save_file == NULL) {
        printf("Erreur !\n");   
        return;        
    }

    fprintf(save_file, "%d\n", game->render_dist);

    for(int i = 0; i < game->map_count; i++){
        fprintf(save_file, "%f\n", game->best_times[i]);
    }
    fclose(save_file);
}

// Mise à jour du chronomètre
void update_timer(game_t* game, float dt){

    if(game->game_state == GAME){
        game->current_time += dt;
    }
}

void free_game(game_t* game){

    free(game);
}