 /**
 * @file sokoban.c
 * @brief Programme du jeux sokoban
 * @author Antoine Schoubert
 * @version 2.0
 * @date 9/10/2025
 * 
 * Programme qui a permit de crée le jeux sokoban, il permet aussi d'y jouer
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

/**** Define ****/
#define TAILLE 12
#define MAX 500
#define HAUT 'z'
#define BAS 's'
#define DROITE 'd'
#define GAUCHE 'q'
#define ABANDON 'x'
#define RECOMMANCE 'r'


/**** Types ****/
typedef char tPlateau[TAILLE][TAILLE];
typedef char tTabDeplacement[MAX];

typedef struct{
    int posX;
    int posY;
}t_joueur;

typedef struct{
    int x;
    int y;
}t_deplacement;

/**** Constantes ****/

const char PERSO = '@';
const char VIDE = ' ';
const char CAISSE = '$';
const char CIBLE = '.';
const char MUR = '#';
const char PERSO_SUR_CIBLE = '+';
const char CAISSE_SUR_CIBLE = '*';

const char OUI = 'o';
const char NON = 'n';

const char ZOOM_IN = '+';
const char ZOOM_OUT = '-';

const char UNDO = 'u';

const char SOKO_SEUL_GAUCHE = 'g';
const char SOKO_SEUL_HAUT = 'h';
const char SOKO_SEUL_BAS = 'b';
const char SOKO_SEUL_DROITE = 'd';

const char SOKO_CAISSE_GAUCHE = 'G';
const char SOKO_CAISSE_HAUT = 'H';
const char SOKO_CAISSE_BAS = 'B';
const char SOKO_CAISSE_DROITE = 'D';



/***** Entete fonction *****/
int kbhit();
void afficher_entete(char[]);
void charger_partie(tPlateau, char[]);
void afficher_plateau(tPlateau, int*);
void recherche_pos_joueur(tPlateau, t_joueur*);
void gerer_deplacement(char, t_deplacement*, tPlateau, bool*, char nomNiveau[], int *zoom, t_joueur *joueur);
void deplacer(tPlateau, t_joueur*, t_deplacement);
void deplacement_caisse(tPlateau, int, int, int, int, t_joueur*);
void zoom_in_out( char, int*, tPlateau, char[]);
bool gagner(tPlateau);
void enregistrer_partie(tPlateau, char[]);
void abandonner(tPlateau);
void recommancer(tPlateau, char[], int*, t_joueur*);

/***** Fonction principal *****/
int main(){ 

    tPlateau platJeu;
    t_joueur joueur;
    t_deplacement dep;

    char nomNiveau[25];
    char touche;

    int zoom = 1; 

    bool abandon = false;


    printf("Entre un niveau \n");
    scanf("%s", nomNiveau);
    strcat(nomNiveau, ".sok");

    charger_partie(platJeu, nomNiveau);

    afficher_entete(nomNiveau);
    afficher_plateau(platJeu, &zoom);

    recherche_pos_joueur(platJeu, &joueur);

    while((gagner(platJeu) != true) && (abandon != true)){

        if(kbhit()){
            touche = getchar();

            gerer_deplacement(touche, &dep, platJeu, &abandon, nomNiveau, &zoom, &joueur);
            deplacer(platJeu, &joueur, dep);


            zoom_in_out(touche, &zoom, platJeu, nomNiveau);
            afficher_entete(nomNiveau);
            afficher_plateau(platJeu, &zoom);
            
        }
        
    }

    return EXIT_SUCCESS;

}



void afficher_entete(char nomNiveau[]){
    system("clear");
    printf("%s \n\n", nomNiveau);
    printf("Z  Déplacement vers le haut \n");
    printf("Q  Déplacement vers la gauche \n");
    printf("S  Déplacement vers le bas \n");
    printf("D  Déplacement vers la droite \n");
    printf("X  Abandonner \n");
    printf("R  Recommencer \n");
    printf("+  Zommer\n");
    printf("-  Dezoomer\n");
}

void afficher_plateau(tPlateau plateau, int *zoom){
    for(int x = 0 ; x < TAILLE ; x++){
        
	for(int j = 0 ; j < (*zoom) ; j++){
	    
		for(int y = 0 ; y < TAILLE ; y++){

       	for(int k = 0 ; k < (*zoom) ; k++){

			if(plateau[x][y] == PERSO_SUR_CIBLE){

               	printf("%c", PERSO);
      		}
       		else if(plateau[x][y] == CAISSE_SUR_CIBLE){
               
           		printf("%c", CAISSE);            
       		}
       		else{
           		// convertit du decimal en caractere
           		printf("%c", plateau[x][y]);
       		}
       	}
    	}
 	printf("\n"); 
	}    
	}
	printf("\n");
}

void recherche_pos_joueur(tPlateau plateau, t_joueur *joueur){
    for(int x = 0 ; x < TAILLE ; x++){
        for(int y = 0 ; y < TAILLE ; y++){
            if(plateau[x][y] == PERSO){
                joueur->posX = x;
                joueur->posY = y;
            }
        }
    }
}

void gerer_deplacement(char touche, t_deplacement *dep, tPlateau platJeu ,bool *abandon, char nomNiveau[], int *zoom, t_joueur *joueur){
    switch(touche){
        case HAUT:
            dep->x = -1;
            dep->y = 0;
            break;
        
        case BAS:
            dep->x = 1;
            dep->y = 0;
            break;
        
        case DROITE:
            dep->x = 0;
            dep->y = 1;
            break;
        
        case GAUCHE: 
            dep->x = 0;
            dep->y = -1;
            break;

        case ABANDON:
            abandonner(platJeu);
            *abandon = true;
            break;
        
        case RECOMMANCE:
            recommancer(platJeu, nomNiveau, zoom, joueur);
            break;

        default:
        dep->x = 0;
        dep->y = 0;
    }
}

void deplacer(tPlateau platJeu, t_joueur *joueur, t_deplacement dep){
    int posSuivanteX = joueur->posX + dep.x;
    int posSuivanteY = joueur->posY + dep.y;

    int posApresSuivanteX = posSuivanteX + dep.x;
    int posApresSuivanteY = posSuivanteY + dep.y;

    if(dep.x == 0 && dep.y == 0){
        joueur->posX += 0;
        joueur->posY += 0;
    }

    if(platJeu[posSuivanteX][posSuivanteY] != MUR && (posSuivanteX < TAILLE 
        && posSuivanteY < TAILLE) && (posSuivanteX >= 0 && posSuivanteY >= 0)){

        if(platJeu[posSuivanteX][posSuivanteY] != CAISSE &&
            platJeu[posSuivanteX][posSuivanteY] != CAISSE_SUR_CIBLE){

            if(platJeu[posSuivanteX][posSuivanteY] == CIBLE){
                platJeu[posSuivanteX][posSuivanteY] = PERSO_SUR_CIBLE;
            }
            else{
                platJeu[posSuivanteX][posSuivanteY] = PERSO;
            }

            if(platJeu[joueur->posX][joueur->posY] == PERSO_SUR_CIBLE){
                platJeu[joueur->posX][joueur->posY] = CIBLE;
            }
            else{
                platJeu[joueur->posX][joueur->posY] = VIDE;
            }
            
            joueur->posX += dep.x;
            joueur->posY += dep.y;
        }

        else if(platJeu[posApresSuivanteX][posApresSuivanteY] != MUR && 
                platJeu[posApresSuivanteX][posApresSuivanteY] != CAISSE && 
                platJeu[posApresSuivanteX][posApresSuivanteY] != CAISSE_SUR_CIBLE){

                    deplacement_caisse(platJeu, posApresSuivanteX, posApresSuivanteY, posSuivanteX, posSuivanteY, joueur);

                    joueur->posX += dep.x;
                    joueur->posY += dep.y;
                }
        
    }
}

void deplacement_caisse(tPlateau platJeu, int posApresSuivanteX, int posApresSuivanteY, int posSuivanteX, int posSuivanteY, t_joueur *joueur){

    if(platJeu[posApresSuivanteX][posApresSuivanteY] == CIBLE){
        platJeu[posApresSuivanteX][posApresSuivanteY] = CAISSE_SUR_CIBLE;
    }
    else{
        platJeu[posApresSuivanteX][posApresSuivanteY] = CAISSE;
    }

    if(platJeu[posSuivanteX][posSuivanteY] == CIBLE || platJeu[posSuivanteX][posSuivanteY] == CAISSE_SUR_CIBLE){
        platJeu[posSuivanteX][posSuivanteY] = PERSO_SUR_CIBLE;
    }
    else{
        platJeu[posSuivanteX][posSuivanteY] = PERSO;
    }

    if(platJeu[joueur->posX][joueur->posY] == PERSO_SUR_CIBLE){
       platJeu[joueur->posX][joueur->posY] = CIBLE;
    }
    else{
        platJeu[joueur->posX][joueur->posY] = VIDE;
    }
}


void zoom_in_out( char touche, int *zoom, tPlateau plateau, char nomNiveau[]){
	if ( touche == ZOOM_IN  && (*zoom) < 3){
		(*zoom)++;
		system("clear");
		afficher_entete(nomNiveau);
		afficher_plateau(plateau,zoom);
	}
	else if(touche == ZOOM_OUT && (*zoom) > 1){
		(*zoom)--;
		system("clear");
		afficher_entete(nomNiveau);
		afficher_plateau(plateau,zoom);
	}
}


bool gagner(tPlateau plateau){

    for(int x = 0 ; x < TAILLE ; x++){
        for(int y = 0; y < TAILLE ; y++){
            if(plateau[x][y] == CIBLE || plateau[x][y] == PERSO_SUR_CIBLE){

                return false;
                
            }
        }
    }
    return true;
}

/**
 * @brief procedure qui permet d'abandonner et d'enregisré si voulu la partie
 * @param platJeu de type caracteres, E/S : recoit le plateau de jeu
 */
void abandonner(tPlateau platJeu){
    
    char niveau[26];
    char verife;


    system("clear");
    printf("Voulez vous enregistrez (\"o\" pour oui, \"n\" pour non) ? \n");
    scanf("%c", &verife);

    if(verife == OUI){

        printf("Donnew un nom de fichier (max 25 car) \n");
        scanf("%s", niveau);
        strcat(niveau, ".sok");
        
        enregistrer_partie(platJeu,niveau);
        printf(("vous avez abandonner\n"));

    }

}

void recommancer(tPlateau platJeu, char fichier[], int *zoom, t_joueur *joueur){
    
    char verife;


    system("clear");
    printf("Voulez vous recommancer (\"o\" pour oui, \"n\" pour non) ? \n");
    scanf("%c", &verife);

    if(verife == OUI){

        system("clear");
        charger_partie(platJeu, fichier);
        afficher_entete(fichier);
        afficher_plateau(platJeu, zoom);
        recherche_pos_joueur(platJeu, joueur);
    }
    

}



/****** FONCTION A NE PAS TOUCHER  ******/
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


void charger_partie(tPlateau plateau, char fichier[]){
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

void enregistrer_partie(tPlateau plateau, char fichier[]){
    FILE * f;
    char finDeLigne='\n';

    f = fopen(fichier, "w");
    for (int ligne=0 ; ligne<TAILLE ; ligne++){
        for (int colonne=0 ; colonne<TAILLE ; colonne++){
            fwrite(&plateau[ligne][colonne], sizeof(char), 1, f);
        }
        fwrite(&finDeLigne, sizeof(char), 1, f);
    }
    fclose(f);
}
