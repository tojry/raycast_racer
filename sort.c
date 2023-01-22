#include "sort.h"
#include "obstacle.h"

// Echange de 2 valeurs
void swap(obstacle_t** tab, int i, int j){
    if(i != j){
        obstacle_t* temp = tab[i];
        tab[i] = tab[j];
        tab[j] = temp;
    }
}

// Séparation du tableau
int split(obstacle_t** tab, int lower, int upper){
    int left = lower-1;
    int right = upper + 1;
    obstacle_t* pivot = tab[lower];
    while(left < right){
        left++;
        while(tab[left]->player_dst < pivot->player_dst) left++;
        right--;
        while(tab[right]->player_dst > pivot->player_dst) right--;
        if(left < right){
            swap(tab, left, right);
        }
    }
    return right;
}

// Quicksort d'une liste d'obstacle en fonction de leur distance
void sort(obstacle_t** tab, int lower, int upper){
    if(lower < upper){
        int pivot = split(tab, lower, upper);
        sort(tab, lower, pivot);
        sort(tab, pivot+1, upper);
    }
}