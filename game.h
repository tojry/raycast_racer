#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include "player.h"
#include "roads.h"
#include "obstacle.h"

typedef enum {
    MENU,
    LOADING,
    GAME,
    SETTINGS,
    SELECT_MAP,
    FINISH_RACE,
    QUIT
} game_state_e;

typedef struct {

    game_state_e game_state;
    bool end;
    int render_dist;
    float current_time;
    int map_count;
    float best_times[100];
    int current_map;
    bool start_new_race;
    bool new_best_time;

    char* map;
    road_vertex_t** roads;
    player_t* player;
    int map_size;
    int obstacle_count;
    obstacle_t* obstacles[100];

} game_t;

game_t* init_game();
void init_race(game_t* game);
void update_render_distance(game_t* game, int val);
void reach_finish_line(game_t* game);
void clear_race_data(game_t* game);
bool load(game_t* game);
void save(game_t* game);
void update_timer(game_t* game, float dt);
void free_game(game_t* game);

#endif