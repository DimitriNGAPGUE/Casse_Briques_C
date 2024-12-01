#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphics.h"

#define largeurPalette 100 
#define hauteurPalette 10 
#define rayonBalle 8

// Fonction pour déplacer la palette
void move_palette(POINT *p1, POINT *p2, int dx) {
    p1->x += dx;
    p2->x += dx;
    
    // Empêcher la palette de sortir du côté gauche
    if (p1->x < 0) {
        p1->x = 0;
        p2->x = largeurPalette;
    }
    
    // Empêcher la palette de sortir du côté droit
    if (p2->x > 1000) {  // La limite est 1000 (largeur de la fenêtre)
        p2->x = 1000;
        p1->x = 1000 - largeurPalette;
    }
}

// Fonction pour vérifier la collision de la balle avec les briques
int check_collision_brique(POINT balle, char ***array, int *score) {
    int i, j;
    int briqueDetruite = 0;
    for (i = 1; i < 10; i++) {  // Zone des briques
        for (j = 0; j < 33; j++) {
            if (array[i][j] != NULL && strcmp(array[i][j], "bbb") == 0) {
                POINT p1, p2;
                p1.x = j * 30;
                p1.y = 600 - (i * 20);
                p2.x = p1.x + 28;
                p2.y = p1.y - 18;
                
                if (balle.x >= p1.x && balle.x <= p2.x && balle.y >= p2.y && balle.y <= p1.y) {
                    free(array[i][j]);  // Libérer la mémoire allouée à la brique
                    array[i][j] = NULL;  // Supprimer la brique
                    (*score)++;  // Augmenter le score
                    briqueDetruite = 1;  // Signaler qu'une brique a été détruite
                    return 1;
                }
            }
        }
    }
    return briqueDetruite;
}

int main() {
    init_graphics(1000, 600);
    affiche_auto_off();

    // Initialisation de la palette
    int positionXPalette = (1000 - largeurPalette) / 2;
    int positionYPalette = 20;

    // Initialisation de la balle
    POINT balle;
    balle.x = 500;
    balle.y = 100;

    int balleDirX = 5;  // Vitesse horizontale de la balle
    int balleDirY = 5;  // Vitesse verticale de la balle

    // Grille des briques
    int n = 33;  // Nombre de lignes
    int m = 33;  // Nombre de colonnes
    char ***array;
    int i, j;

    // Allocation dynamique de mémoire pour la grille de briques
    array = (char ***) malloc(n * sizeof(char **));
    for (i = 0; i < n; i++) {
        array[i] = (char **) malloc(m * sizeof(char *));
        for (j = 0; j < m; j++) {
            array[i][j] = NULL;  // Initialisation à NULL
        }
    }

    // Initialisation des briques
    for (i = 1; i < 5; i++) {
        for (j = 1; j < m - 1; j++) {
            array[i][j] = strdup("bbb");
        }
    }

    int score = 0;  // Variable de score

    POINT point1, point2;
    point1.x = positionXPalette;
    point1.y = positionYPalette;
    point2.x = positionXPalette + largeurPalette;
    point2.y = positionYPalette + hauteurPalette;

    int dx = 0;  // Vitesse de déplacement de la palette

    while (1) {
        fill_screen(noir);  // Effacer l'écran

        // Dessiner les briques
        POINT p1, p2;
        for (i = 1; i < 10; i++) { 
            for (j = 0; j < m - 1; j++) {
                if (array[i][j] != NULL && strcmp(array[i][j], "bbb") == 0) {
                    p1.x = j * 30;  
                    p1.y = 600 - (i * 20);  
                    p2.x = p1.x + 28;
                    p2.y = p1.y - 18;
                    draw_fill_rectangle(p1, p2, vert);
                }
            }
        }

        // Utiliser get_arrow() pour obtenir les entrées du clavier
        POINT key = get_arrow();

        // Détecter quelle flèche est appuyée
        if (key.x == -1) {  // Flèche gauche
            dx = -50;  // Déplacement à gauche
        } else if (key.x == 1) {  // Flèche droite
            dx = 50;  // Déplacement à droite
        } else {
            dx = 0;  // Si aucune touche n'est pressée
        }

        // Déplacer la palette de façon continue
        move_palette(&point1, &point2, dx);

        // Déplacer la balle
        balle.x += balleDirX;
        balle.y += balleDirY;

        // Vérifier les collisions avec les murs
        if (balle.x <= 0 || balle.x >= 1000) {
            balleDirX = -balleDirX;
        }
        if (balle.y >= 600) {
            balleDirY = -balleDirY;
        }

        // Collision avec la palette
        if (balle.y - rayonBalle <= positionYPalette + hauteurPalette && balle.x >= point1.x && balle.x <= point2.x) {
            balleDirY = -balleDirY;
        }

        // Collision avec le sol (Game Over)
        if (balle.y <= 0) {
            printf("Game Over! Score: %d\n", score);
            break;
        }

        // Collision avec les briques
        if (check_collision_brique(balle, array, &score)) {
            balleDirY = -balleDirY;  // Rebondir après destruction de la brique
        }

        // Dessiner la balle
        draw_fill_circle(balle, rayonBalle, rouge);

        // Dessiner la palette
        draw_fill_rectangle(point1, point2, jaune);

        // Rafraîchir l'affichage
        affiche_all();

        // Pause pour ralentir la boucle
        usleep(30000);  // Pause de 30ms
    }

    // Libération de la mémoire des briques
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            if (array[i][j] != NULL) {
                free(array[i][j]);
            }
        }
        free(array[i]);
    }
    free(array);

    return 0;
}
