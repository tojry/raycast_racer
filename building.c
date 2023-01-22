#include <string.h>
#include <stdio.h>

#include "SDL_utils.h"
#include "building.h"

void load_buildings(building_t** buildings){

    char building_file_name[35] = "buildings/building_0.txt";
    for(int i = 0; i < BUILDINGS_COUNT; i++){
        sprintf(building_file_name, "buildings/building_%d.txt", i);
        buildings[i] = load_building(building_file_name);
    }
}

// Chargement d'un bâtiment depuis un fichier
building_t* load_building(char* file_name){

    FILE* building_file;
    
    building_file = fopen(file_name, "r");
    if(building_file == NULL){
        
        printf("Erreur lors de l'ouverture du fichier\n");
    }

    // Nom du fichier de la texture du batiment
    char tex_name[100];
    fgets(tex_name, 100, building_file);
    tex_name[strcspn(tex_name, "\r\n")] = 0;

    // Largeur de la texture du batiment
    char str_tex_w[5];
    fgets(str_tex_w, 5, building_file);
    int tex_w;
    sscanf(str_tex_w, "%d", &tex_w);

    // Hauteur de la texture du batiment
    char str_tex_h[5];
    fgets(str_tex_h, 5, building_file);
    int tex_h;
    sscanf(str_tex_h, "%d", &tex_h);

    // Hauteur du batiment
    char str_building_h[5];
    fgets(str_building_h, 5, building_file);
    float building_h;
    sscanf(str_building_h, "%f", &building_h);

    fclose(building_file);

    uint32_t* tex = load_building_texture(tex_name);

    building_t* building = (building_t*)malloc(sizeof(building_t));
    building->texture_name = tex_name;
    building->texture_width = tex_w;
    building->texture_height = tex_h;
    building->building_height = building_h;
    building->texture = tex;

    return building;
}

// Chargement de la texture d'un bâtiment
uint32_t* load_building_texture(char* file_name){

    SDL_Surface* surface = load_png_surface(file_name);
    
    int tex_w = surface->w;
    int tex_h = surface->h;

    uint32_t* img = (uint32_t*)malloc(sizeof(uint32_t) * tex_w * tex_h);
    
    for(int i = 0; i < tex_w; i++){
        for(int j = 0; j < tex_h; j++){
            uint8_t r = ((uint8_t*)(surface->pixels))[(i+j*tex_w)*4+3];
            uint8_t g = ((uint8_t*)(surface->pixels))[(i+j*tex_w)*4+2];
            uint8_t b = ((uint8_t*)(surface->pixels))[(i+j*tex_w)*4+1];
            uint8_t a = ((uint8_t*)(surface->pixels))[(i+j*tex_w)*4+0];
            img[i+j*tex_w] = convert_color(r, g, b, a);
        }
    }
    
    SDL_FreeSurface(surface);

    return img;
}

void free_building(building_t* building){

    free(building->texture);
    free(building);
}

void free_buildings(building_t** buildings, int n){

    for(int i = 0; i < n; i++)
        free_building(buildings[i]);
}