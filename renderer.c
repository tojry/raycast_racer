#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "SDL_utils.h"
#include "renderer.h"
#include "sort.h"
#include "map.h"

// Initialisation de la texture du rendu
uint32_t* init_texture(int width, int height, uint32_t color){

    uint32_t* texture = (uint32_t*)malloc(sizeof(uint32_t) * width * height);
    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            
            texture[x + y*width] = color;
        }
    }
    return texture;
}

// Affichage d'une colonne de pixel
void draw_column(uint32_t* texture, int width, int height, building_t* building, int tex_coord, int x, int min_y, int max_y, int max_y_ground, float current_height){

    int column_height = abs(max_y_ground - min_y);

    for (int y = 0; y < height; y++) {

        if(y < min_y){
            texture[x + y * width] = SKY_COLOR;
        }
        else if(y >= max_y){
            if(current_height < 1.0)
                texture[x + y * width] = GROUND_COLOR;
        }
        else{
            int pix_x = tex_coord;
            int pix_y = ((y - min_y) * building->texture_height) / column_height;

            texture[x + y * width] = building->texture[pix_x + pix_y * building->texture_width];
        }
    }
}

void clear_texture(uint32_t* texture, int width, int height){

    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            texture[x + y*width] = y > height * 0.1 ? GROUND_COLOR : SKY_COLOR;
        }
    }
}

// Rendu 3D
void render(uint32_t* texture, building_t** buildings, obstacle_t** obstacles, char* map, player_t* player, int width, int height, int map_size, int obstacle_count, int render_dst){

    clear_texture(texture, width, height);

    float depth_buffer[width];
    float inv_width = 1. / (float)width;
    float width_half = width >> 1;

    // Pour chaque colonne de pixel de la fenêtre
    for (int i = 0; i < width; i++) {
        // Angle du rayon
        float angle = (player->cam_angle) - (FOV * 0.5) + FOV * i * inv_width;
        float lod = 0.01;

        // Calcul des valeurs pour l'optimisation
        float cos_angle = cos(angle);
        float sin_angle = sin(angle);
        float cos_angle_minus_cam_angle = cos(angle - (player->cam_angle));

        float current_height = 0.;
        int current_max_y = 0;

        for (float c = 0; c < render_dst; c+=lod) {

            // Position actuelle du rayon
            float x = (player->cam_position->x + c * cos_angle) / MAP_SCALE;
            float y = (player->cam_position->y + c * sin_angle) / MAP_SCALE;

            // On vérifie si le rayon sort de la map
            if(x < 0 || x >= map_size || y < 0 || y >= map_size) break;

            int map_index = (int)x + (int)y * map_size;
            
            // Si le rayon est sur un mur
            if (is_wall(map, map_index)){
                int building_id = map[map_index] - '0';
                float hit_height = buildings[building_id]->building_height;
                
                if(hit_height <= current_height) continue;

                current_height = hit_height;
                
                int texture_width = buildings[building_id]->texture_width;

                // Distance entre le mur et le joueur
                float dst = c * cos_angle_minus_cam_angle;
                
                // Mise à jour du depthbuffer
                if(current_max_y == 0)
                    depth_buffer[i] = dst;

                // Demi-hauteur d'une colonne de pixel
                int column_half_height = (int)((height / dst) * MAP_SCALE * hit_height / 2);

                // Calcul des coordonnées sur le sprite du mur du pixel à afficher
                float hitx = x - floor(x+.5);
                float hity = y - floor(y+.5);
                int tex_coord_x = hitx * texture_width;

                if(fabs(hity) > fabs(hitx))
                    tex_coord_x = hity * texture_width;

                if (tex_coord_x < 0)
                    tex_coord_x += texture_width;

                // Décalage des bâtiments vers le haut de l'écran pour donner l'impression que la caméra est plus proche du sol
                int offset = (int)(height * 0.4) + (hit_height > 1.0 ? (int)(column_half_height - column_half_height / hit_height) : 0);
                int min_y = width_half - column_half_height - offset;
                int max_y = width_half + column_half_height - offset;
                int max_y_ground = max_y;
                if(current_max_y != 0) max_y = current_max_y;
                
                draw_column(texture, width, height, buildings[building_id], tex_coord_x, i, min_y, max_y, max_y_ground, hit_height);
                
                current_max_y = min_y;

                if(hit_height >= MAX_BUILDING_HEIGHT)
                    break;
                
            }
            
            // Réduction de la précision du rayon s'il est éloigné très du joueur
            if(current_height > 0. || c >= 45){
                lod = 0.02;
            }
        }
    }
    for (int i = 0; i < obstacle_count; i++) {
        obstacles[i]->player_dst = sqrt(pow(player->cam_position->x - obstacles[i]->position->x, 2) + pow(player->cam_position->y - obstacles[i]->position->y, 2));
    }
    // Tri du tableau des obstacles en fonction de leur distance
    sort(obstacles, 0, obstacle_count-1);

    // Affichage des obstacles
    for(int i = obstacle_count-1; i >= 0; i--){
        draw_obstacle(texture, width, height, depth_buffer, obstacles[i], player);
    }
}

// Affichage d'un obstacle
void draw_obstacle(uint32_t* texture, int width, int height, float depth_buffer[width], obstacle_t* sprite, player_t* player){

    // Direction du sprite par rapport au joueur
    float sprite_dir = atan2(sprite->position->y - player->cam_position->y, sprite->position->x - player->cam_position->x);
    while (sprite_dir - player->cam_angle >  PI) sprite_dir -= 2*PI; 
    while (sprite_dir - player->cam_angle < -PI) sprite_dir += 2*PI;

    // Distance entre le sprite et le joueur
    float sprite_dst = distance(sprite->position, player->position);
    // Taille du sprite à afficher
    int sprite_size = fmin(550, (height / sprite_dst) * sprite->height_scale);
    // Calcul de la position du sprite à l'écran
    int x_offset = (sprite_dir - player->cam_angle) * width / FOV + (width >> 1) - (sprite_size >> 1);
    int y_offset = (height >> 1) - (sprite_size >> 1) - (height * 0.4);

    // Direction du sprite en degrés
    float theta = (sprite->direction - sprite_dir) * 180. / PI;
    if(theta < 0) theta = 360 + theta;
    // Index du sprite à afficher en fonction de l'orientation
    int sprite_count = sprite->spritesheet_width * sprite->spritesheet_height;
    int sprite_index = theta / (360. / sprite_count);
    if(sprite_index >= sprite_count) sprite_index = sprite_index % sprite_count;

    // Coordonnées du sprite à afficher sur la spritesheet
    int tex_coord_x = sprite_index % sprite->spritesheet_width;
    int tex_coord_y = sprite_index / sprite->spritesheet_width;

    // Affichage du sprite à l'écran
    for (int i = 0; i < sprite_size; i++) {
        if (x_offset + i < 0 || x_offset + i >= width) continue;
        if (depth_buffer[x_offset + i] < sprite_dst) continue;

        depth_buffer[x_offset + i] = sprite_dst;
        for (int j = 0; j < sprite_size; j++) {
            if (y_offset + j < 0 || y_offset + j >= height) continue;

            // Coordonnées sur la spritesheet du pixel à afficher
            int pix_x = (i * OBSTACLE_TEXTURE_SIZE / sprite_size) + (tex_coord_x * OBSTACLE_TEXTURE_SIZE);
            int pix_y = (j * OBSTACLE_TEXTURE_SIZE / sprite_size) + (tex_coord_y * OBSTACLE_TEXTURE_SIZE);
            uint32_t color = sprite->texture[pix_x + pix_y * OBSTACLE_TEXTURE_SIZE * sprite->spritesheet_width];
            uint8_t r,g,b,a;
            read_color(&color, &r, &g, &b, &a);
            if(a > 4 || r != 0 || g != 0 || b != 0)
                texture[width + x_offset + i + (y_offset+j) * width] = color;
        }
    }
}