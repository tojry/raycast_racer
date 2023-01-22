#include <stdlib.h>
#include <stdio.h>
#include "map.h"

char* load_map(char* file_name, int* size, int* obstacle_count){
    
    FILE* map_file;
    int c, n = 0;
    
    map_file = fopen(file_name, "r");
    if(map_file == NULL){
        
        printf("Erreur lors de l'ouverture du fichier map\n");
    }
    
    // Taille de la map
    char str_size[5];
    fgets(str_size, 5, map_file);
    sscanf(str_size, "%d", size);

    // Nombre d'obstacles sur la map
    char str_obsctacle_count[5];
    fgets(str_obsctacle_count, 5, map_file);
    sscanf(str_obsctacle_count, "%d", obstacle_count);

    // Contenu de la map
    char* map = (char*)malloc(sizeof(char) * (*size) * (*size));
    do{
        c = fgetc(map_file);
        if((c >= '0' && c <= '9') || c == ' ' || c == '=' || c == 'x'){
            map[n] = c;
            n++;
        }
    }while (c != EOF);
    
    fclose(map_file);

    return map;
}

bool is_wall(char* map, int index){

    return map[index] >= '0' && map[index] <= '9'; 
}

bool is_start(char* map, int index){
    
    return map[index] == 'x';
}

bool is_end(char* map, int index){
    
    return map[index] == '=';
}