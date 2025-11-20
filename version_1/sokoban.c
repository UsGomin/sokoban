#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#define TAILLE 12


typedef char t_plateau[TAILLE][TAILLE];

const char HAUT = 'z';
const char BAS = 's';
const char DROITE = 'd';
const char GAUCHE = 'q';
const char ABANDON = 'x';
const char RECOMMANCE = 'r';


void charger_Partie(t_plateau plateau, char fichier[]);
void affiche_Plateau(t_plateau plateau);
void affiche_Entete(char*);
void Deplacer(char* , int pos_joueur[2], t_plateau plateau, char fichier[], int* , int* , int* , int*, int pos_cible[4]);
bool gagne();
int kbhit();
void Position_Joueur(t_plateau plateau, char fichier[], int pos_joueur[2]);


void haut(t_plateau plateau, int pos_joueur[2], int* , int* , int* , int*, int pos_cible[4]);
void gauche(t_plateau plateau, int pos_joueur[2], int* , int* , int* , int*, int pos_cible[4]);
void droite(t_plateau plateau, int pos_joueur[2], int* , int* , int* , int*, int pos_cible[4]);
void bas(t_plateau plateau, int pos_joueur[2], int *x1, int *x2, int *y1, int *y2, int pos_cible[4]);

void New_Plateau (int pos_joueur[2], t_plateau plateau, int* , int* , int* , int*, int pos_cible[4]);


int main(){

    char touche[10];
    char niveau[26];
    
    int x1, x2, y1, y2;

    x1 = 0;
    x2 = 0;
    y1 = 0;
    y2 = 0;


    int pos_joueur[2];
    int pos_cible[4];


    t_plateau plateau;

    printf("Saisissez un niveau.sok \n");
    scanf("%s", niveau);


    charger_Partie(plateau, niveau);
    affiche_Entete(niveau);
    affiche_Plateau(plateau);
    Position_Joueur(plateau, niveau, pos_joueur);

    //boucle while pour jouer tant qu'on a pas gagne
    while (gagne() != true){

        Deplacer(touche, pos_joueur, plateau, niveau, &x1, &x2, &y1, &y2, pos_cible);

    }
    //fin boucle while car victoire ou abandon
    
    printf("Vous avez abandonner \n");

    return 0;
}



void affiche_Entete(char *niveau){

    system("clear");
    printf("%s \n\n", niveau);
    printf("Z pour aller vers le haut \n");
    printf("S pour aller vers le bas \n");
    printf("Q pour aller vers la gauche \n");
    printf("D pour aller vers la droite \n");
    printf("X pour abandonner \n");
    printf("R pour recommencer \n\n");

}

void affiche_Plateau(t_plateau plateau){

    for (int x = 0 ; x < TAILLE ; x++){
        for (int y = 0 ; y < TAILLE ; y++){
            // convertit du decimal en caractere
            printf("%c", plateau[x][y]);
        }
        printf("\n");
    }
}

void Deplacer(char *touche, int pos_joueur[2], t_plateau plateau, char fichier[], int *x1, int *x2, int *y1, int *y2, int pos_cible[4]){


    (*touche) = '\0';
    if (kbhit()){

        *x1 = 0;
        *x2 = 0;
        *y1 = 0;
        *y2 = 0;
        
        (*touche) = getchar();
        if ( (*touche) == HAUT){

            system("clear");
            affiche_Entete(fichier);
            haut(plateau, pos_joueur, x1, x2, y1, y2, pos_cible);


        }
        else if ( (*touche) == BAS) {

            system("clear");
            affiche_Entete(fichier);
            bas(plateau, pos_joueur, x1, x2, y1, y2, pos_cible);
    
        }
        else if ( (*touche) == GAUCHE) {

            system("clear");
            affiche_Entete(fichier);
            gauche(plateau, pos_joueur, x1, x2, y1, y2, pos_cible); 

        }
        else if ( (*touche) == DROITE) {

            system("clear");
            affiche_Entete(fichier);
            droite(plateau, pos_joueur, x1, x2, y1, y2, pos_cible);
            
            
        }
    }
    
}

void Position_Joueur(t_plateau plateau, char fichier[], int pos_joueur[2]){

    for (int x = 0 ; x < TAILLE ; x++){
        for (int y = 0 ; y < TAILLE ; y++){
            if (plateau[x][y] == '@'){
                pos_joueur[0] = x;
                pos_joueur[1] = y;
            }
        }
    }
}



void haut(t_plateau plateau, int pos_joueur[2], int *x1, int *x2, int *y1, int *y2, int pos_cible[4]){

    *x1 = -1;
    *x2 = -2;
    
    New_Plateau(pos_joueur,plateau, x1, x2, y1, y2, pos_cible);

}

void bas(t_plateau plateau, int pos_joueur[2], int *x1, int *x2, int *y1, int *y2, int pos_cible[4]){

    *x1 = 1;
    *x2 = 2;
    

    New_Plateau(pos_joueur,plateau, x1, x2, y1, y2, pos_cible);

}

void droite(t_plateau plateau, int pos_joueur[2], int *x1, int *x2, int *y1, int *y2, int pos_cible[4]){

    *y1 = 1;
    *y2 = 2;
    

    New_Plateau(pos_joueur,plateau, x1, x2, y1, y2, pos_cible);
}



void gauche(t_plateau plateau, int pos_joueur[2], int *x1, int *x2, int *y1, int *y2, int pos_cible[4]){

    *y1 = -1;
    *y2 = -2;
    

    New_Plateau(pos_joueur,plateau, x1, x2, y1, y2, pos_cible);
}

void New_Plateau (int pos_joueur[2], t_plateau plateau, int *x1, int *x2, int *y1, int *y2, int pos_cible[4]){  //mettre x1 y1... ici pour permettre de modifier le tableau, puis dans les fonctions droite...ect, elle modiefiront les valeurs x1,y1... ect
    
        if (plateau[pos_joueur[0] + *x1][pos_joueur[1] + *y1] == ' ' ){  //verfie si la colonne d'a cote est libre en partant de la pos du joueur

            plateau[pos_joueur[0] + *x1][pos_joueur[1] + *y1] = '@'; // si la condition est rempli ecris arobase
            plateau[pos_joueur[0]][pos_joueur[1]] = ' '; //supprime l'arobase a l'ancienne place

            pos_joueur[0] = pos_joueur[0] + *x1;
            pos_joueur[1] = pos_joueur[1] + *y1;
            plateau[pos_cible[0]][pos_cible[1]] = '.';
        }
        else if ( (plateau[pos_joueur[0] + *x1][pos_joueur[1] + *y1] == '$') && (plateau[pos_joueur[0] + *x2][pos_joueur[1] + *y2] == ' ') ){

            plateau[pos_joueur[0] + *x2][pos_joueur[1] + *y2] = '$';
            plateau[pos_joueur[0] + *x1][pos_joueur[1] + *y1] = '@';
            plateau[pos_joueur[0]][pos_joueur[1]] = ' ';

            pos_joueur[0] = pos_joueur[0] + *x1;
            pos_joueur[1] = pos_joueur[1] + *y1;
        }
        else if ( (plateau[pos_joueur[0] + *x1][pos_joueur[1] + *y1] == '$') && (plateau[pos_joueur[0] + *x2][pos_joueur[1] + *y2] == '.') ){

            plateau[pos_joueur[0] + *x2][pos_joueur[1] + *y2] = '$';
            plateau[pos_joueur[0] + *x1][pos_joueur[1] + *y1] = '@';
            plateau[pos_joueur[0]][pos_joueur[1]] = ' ';

            pos_joueur[0] = pos_joueur[0] + *x1;
            pos_joueur[1] = pos_joueur[1] + *y1;
        }
        else if ( (plateau[pos_joueur[0] + *x1][pos_joueur[1] + *y1] == '*') && (plateau[pos_joueur[0] + *x2][pos_joueur[1] + *y2] == ' ') ){

            plateau[pos_joueur[0] + *x2][pos_joueur[1] + *y2] = '$';
            plateau[pos_joueur[0] + *x1][pos_joueur[1] + *y1] = '@';    
            plateau[pos_joueur[0]][pos_joueur[1]] = ' ';

            pos_joueur[0] = pos_joueur[0] + *x1;
            pos_joueur[1] = pos_joueur[1] + *y1;
        }
        else if (plateau[pos_joueur[0] + *x1][pos_joueur[1] + *y1] == '.'){

            pos_cible[0] = pos_joueur[0] + *x1;
            pos_cible[1] = pos_joueur[1] + *y1;

            
            plateau[pos_joueur[0] + *x1][pos_joueur[1] + *y1] = '@';
            plateau[pos_joueur[0]][pos_joueur[1]] = ' ';

            pos_joueur[0] = pos_joueur[0] + *x1;
            pos_joueur[1] = pos_joueur[1] + *y1;

        }

        affiche_Plateau(plateau);
}

bool gagne(){

    return false;
}


void charger_Partie(t_plateau plateau, char fichier[]){
    FILE * f;
    char finDeLigne;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("ERREUR SUR FICHIER");
        exit(EXIT_FAILURE);
    } else {
        for (int ligne=0 ; ligne<TAILLE ; ligne++){
            for (int colonne=0 ; colonne<TAILLE ; colonne++){
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}

int kbhit(){
	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere présent
	int unCaractere=0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
	ch = getchar();

	// restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
	if(ch != EOF){
		ungetc(ch, stdin);
		unCaractere=1;
	} 
	return unCaractere;
}

