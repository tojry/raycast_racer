#include <stdio.h>
#include <math.h>

#include "player.h"
#include "map.h"

player_t* init_player(vector2_t* position){

    player_t* player = (player_t*)malloc(sizeof(player_t));
    player->position = position;
    player->turn_angle = 0;
    player->move_velocity = 0;

    player->cam_position = init_vector2(position->x - CAMERA_DISTANCE, position->y);
    player->cam_angle = 3.14/2.;

    player->accelerating = false;
    player->braking = false;
    player->turning_left = false;
    player->turning_right = false;
    player->on_finish_line = false;

    player->wheels_pos = (vector2_t**)malloc(sizeof(vector2_t*) * 4);
    for(int i = 0; i < 4; i++){
        player->wheels_pos[i] = init_vector2(0,0);
    }
    update_wheels_pos(player);

    return player;
}

// Evenements clavier du joueur
void handle_player_inputs(player_t* player, SDL_Event event){

    if(event.type == SDL_KEYDOWN && event.key.repeat == 0){
        switch(event.key.keysym.sym)
        {
            case SDLK_z:
                player->accelerating = true;
                break;
            case SDLK_s:
                player->braking = true;
                break;
            case SDLK_d:
                player->turning_right = true;
                break;
            case SDLK_q:
                player->turning_left = true;
                break;
        }
    }

    if(event.type == SDL_KEYUP && event.key.repeat == 0){
        switch(event.key.keysym.sym)
        {
            case SDLK_z:
                player->accelerating = false;
                break;
            case SDLK_s:
                player->braking = false;
                break;
            case SDLK_d:
                player->turning_right = false;
                break;
            case SDLK_q:
                player->turning_left = false;
                break;
        }
    }
}

// Mise à jour de la position de chaque roue
void update_wheels_pos(player_t* player){

    float dir = player->cam_angle;
    float center_x = player->position->x;
    float center_y = player->position->y;
    float half_width = CAR_WIDTH * 0.5;
    float half_length = WHEEL_SPACING * 0.5;

    player->wheels_pos[0]->x = center_x + (half_length * cos(dir) - half_width * sin(dir));
    player->wheels_pos[0]->y = center_y + (half_length * sin(dir) + half_width * cos(dir));

    player->wheels_pos[1]->x = center_x + (half_length * cos(dir) + half_width * sin(dir));
    player->wheels_pos[1]->y = center_y + (half_length * sin(dir) - half_width * cos(dir));

    player->wheels_pos[2]->x = center_x - (half_length * cos(dir) - half_width * sin(dir));
    player->wheels_pos[2]->y = center_y - (half_length * sin(dir) + half_width * cos(dir));

    player->wheels_pos[3]->x = center_x - (half_length * cos(dir) + half_width * sin(dir));
    player->wheels_pos[3]->y = center_y - (half_length * sin(dir) - half_width * cos(dir));
}

void update_player_car(char* map, int map_size, player_t* player, obstacle_t** obstacles, int obstacle_count, float dt){

    // Mise à jour de l'angle des roues avant
    update_turn_angle(player);

    update_wheels_pos(player);

    // Test de collision avec un obstacle
    bool obstacle_collision = false;
    for(int i = 0; i < obstacle_count; i++){
        if(check_collision_obstacle(player, obstacles[i])){
            obstacle_collision = true;
            break;
        }
    }

    // Test de collision ente un mur et l'avant de la voiture
    bool collision_front = (check_collision_wall(map, map_size, player->wheels_pos[0]->x, player->wheels_pos[0]->y)
                         || check_collision_wall(map, map_size, player->wheels_pos[1]->x, player->wheels_pos[1]->y)
                         || obstacle_collision)
                         && player->move_velocity >= 0.;

    // Calcul des coordonées du point de collsion à l'arrière pour éviter que la caméra passe à travers les murs
    float left_collision_check_x =  player->position->x - (CAMERA_DISTANCE * cos(player->cam_angle) - CAR_WIDTH * 0.5 * sin(player->cam_angle));
    float left_collision_check_y =  player->position->y - (CAMERA_DISTANCE * sin(player->cam_angle) + CAR_WIDTH * 0.5 * cos(player->cam_angle));
    float right_collision_check_x =  player->position->x - (CAMERA_DISTANCE * cos(player->cam_angle) + CAR_WIDTH * 0.5 * sin(player->cam_angle));
    float right_collision_check_y =  player->position->y - (CAMERA_DISTANCE * sin(player->cam_angle) - CAR_WIDTH * 0.5 * cos(player->cam_angle));

    // Test de collision entre un mur et l'arrière de la voiture
    bool collision_back = (check_collision_wall(map, map_size, left_collision_check_x, left_collision_check_y)
                        || check_collision_wall(map, map_size, right_collision_check_x, right_collision_check_y)
                        || obstacle_collision)
                        && player->move_velocity <= 0.;

    // On arrete d'accélerer ou de reculer s'il y a collision par l'avant ou par l'arrière
    if(collision_front || collision_back){
        player->move_velocity = 0;
        if(collision_front){
            player->accelerating = false;
        }
        if(collision_back){
            player->braking = false;
        }
    }

    // Mise à jour de la vitesse en fonction de l'entrée clavier
    if(player->accelerating)
        player->move_velocity += ACCELERATION_RATE;
    else if(player->braking)
        player->move_velocity -= BRAKING_FORCE;

    // On vérifie si la vitesse maximale n'est pas atteinte
    if(player->move_velocity > MAX_SPEED)
        player->move_velocity = MAX_SPEED;
    if(player->move_velocity < -MAX_SPEED_BACKWARD)
        player->move_velocity = -MAX_SPEED_BACKWARD;

    // On applique la friction du sol
    if(!player->accelerating && !player->braking){
        if(fabs(player->move_velocity) > MIN_SPEED){
            player->move_velocity *= FRICTION;
        }else{
            player->move_velocity = 0;
        }
    }

    //Calcul nouvelles positions des roues
    if(!collision_front && !collision_back){
        // Roues arrières
        player->wheels_pos[2]->x += player->move_velocity * cos(player->cam_angle) * dt;
        player->wheels_pos[2]->y += player->move_velocity * sin(player->cam_angle) * dt;
        player->wheels_pos[3]->x += player->move_velocity * cos(player->cam_angle) * dt;
        player->wheels_pos[3]->y += player->move_velocity * sin(player->cam_angle) * dt;

        // Roues avant
        player->wheels_pos[0]->x += player->move_velocity * cos(player->cam_angle + player->turn_angle) * dt;
        player->wheels_pos[0]->y += player->move_velocity * sin(player->cam_angle + player->turn_angle) * dt;
        player->wheels_pos[1]->x += player->move_velocity * cos(player->cam_angle + player->turn_angle) * dt;
        player->wheels_pos[1]->y += player->move_velocity * sin(player->cam_angle + player->turn_angle) * dt;
    }

    // Milieu des roues avant
    vector2_t* front_wheel = init_vector2((player->wheels_pos[0]->x + player->wheels_pos[1]->x) * 0.5, (player->wheels_pos[0]->y + player->wheels_pos[1]->y) * 0.5);
    // Milieu des roues arrières
    vector2_t* back_wheel = init_vector2((player->wheels_pos[2]->x + player->wheels_pos[3]->x) * 0.5, (player->wheels_pos[2]->y + player->wheels_pos[3]->y) * 0.5);

    // Calcul nouvelle position du centre de la voiture
    player->position->x = (front_wheel->x + back_wheel->x) * 0.5;
    player->position->y = (front_wheel->y + back_wheel->y) * 0.5;

    // Calcul nouvelle orientation et position de la camera
    player->cam_angle = atan2(front_wheel->y - back_wheel->y, front_wheel->x - back_wheel->x);
    player->cam_position->x = player->position->x - CAMERA_DISTANCE * cos(player->cam_angle);
    player->cam_position->y = player->position->y - CAMERA_DISTANCE * sin(player->cam_angle);

    free(front_wheel);
    free(back_wheel);
}

// Mise à jour de l'angle des roues avant
void update_turn_angle(player_t* player){

    if(player->turning_left)
        player->turn_angle = -TURN_RATE / (1. + .4 * fabs(player->move_velocity));
    else if(player->turning_right)
        player->turn_angle = TURN_RATE / (1. + .4 * fabs(player->move_velocity));
    else
        player->turn_angle = 0.;
}

// Test de collision avec un mur
bool check_collision_wall(char* map, int map_size, float pos_x, float pos_y){

    int index = (int)(pos_x / MAP_SCALE) + (int)(pos_y / MAP_SCALE) * map_size;
    return is_wall(map, index);
}

// Test de collision avec la ligne d'arrivée
bool check_collision_finish(char* map, int map_size, float pos_x, float pos_y){

    int index = (int)(pos_x / MAP_SCALE) + (int)(pos_y / MAP_SCALE) * map_size;
    return is_end(map, index);
}

// Test de collision avec un obstacles
// Utilisation du théoreme des axes séparateurs (axis separation theorem)
bool check_collision_obstacle(player_t* player, obstacle_t* obstacle){

    vector2_t* axis_to_test[4] = { init_vector2(cos(player->cam_angle), sin(player->cam_angle)), 
                                   init_vector2(-sin(player->cam_angle), cos(player->cam_angle)),
                                   init_vector2(cos(obstacle->direction), sin(obstacle->direction)),
                                   init_vector2(-sin(obstacle->direction), cos(obstacle->direction)) }; 

    bool collision = true;

    for(int i = 0; i < 4; i++){
        float min_player, max_player, min_obstacle, max_obstacle;
        sat(axis_to_test[i], player->wheels_pos, &min_player, &max_player);
        sat(axis_to_test[i], obstacle->corners, &min_obstacle, &max_obstacle);
        free_vector2(axis_to_test[i]);
        if(!overlaps(min_player, max_player, min_obstacle, max_obstacle)){
            collision = false;
        }
    }
    return collision;
}

// Test du théorème des axes séprateurs (axis separation theorem)
void sat(vector2_t* axis, vector2_t** corners, float* min, float* max){

    *min = 1000;
    *max = -1000;
    for(int i = 0; i < 4; i++){
        float dot_val = dot(corners[i], axis);
        if(dot_val < *min) *min = dot_val;
        if(dot_val > *max) *max = dot_val;
    }
}

// Test de chevauchement des segments des extrémités projetés orthogonaux 
// des deux objets dont on teste la collision 
bool overlaps(float min1, float max1, float min2, float max2){

    return (min1 <= min2 && min2 <= max1) || (min2 <= min1 && min1 <= max2);
}

// Libération de la mémoire
void free_player(player_t* player){

    free_vector2(player->position);
    free_vector2(player->cam_position);
    for(int i = 0; i < 4; i++){
        free_vector2(player->wheels_pos[i]);
    }
    free(player->wheels_pos);
    free(player);
}