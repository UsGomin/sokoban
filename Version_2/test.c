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

#define TAILLE 12
#define MAX 500

typedef char tPlateau[TAILLE][TAILLE];
typedef char tTabDeplacement[MAX];

const char HAUT = 'w';
const char BAS = 's';
const char DROITE = 'd';
const char GAUCHE = 'a';
const char ABANDON = 'x';
const char RECOMMANCE = 'r';

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


void charger_partie(tPlateau plateau, char fichier[]);
void enregistrer_partie(tPlateau plateau, char fichier[]);
void affiche_plateau(tPlateau plateau, int*);
void affiche_entete(char*, int*);
void deplacer(char touche, int posJoueur[2], tPlateau plateau, char fichier[], int* , int* , int* , int*, int*, tPlateau copiePlateau, int*,
    char*, char*, tTabDeplacement deplacement, bool*, bool*);
bool gagne(tPlateau plateau, int *nbrCoups);
int kbhit();
void position_joueur(tPlateau plateau, char fichier[], int posJoueur[2]);

void abandonner(tPlateau copiePlateau);

void haut(tPlateau plateau, int posJoueur[2], int* , int* , int* , int*, int*, char*, char*, tTabDeplacement deplacement, bool*, bool*, int*);
void gauche(tPlateau plateau, int posJoueur[2], int* , int* , int* , int*, int*, char*, char*, tTabDeplacement deplacement, bool*, bool*, int*);
void droite(tPlateau plateau, int posJoueur[2], int* , int* , int* , int*, int*, char*, char*, tTabDeplacement deplacement, bool*, bool*, int*);
void bas(tPlateau plateau, int posJoueur[2], int*, int*, int*, int*, int*, char*, char*, tTabDeplacement deplacement, bool*, bool*, int*);

void new_plateau(int posJoueur[2], tPlateau plateau, int* , int* , int* , int*, int*, bool*, bool*);

void remplace_car(tPlateau plateau, tPlateau copiePlateau, int posJoueur[2]);

void recommancer(tPlateau plateau, char fichier[], int *nbrCoups, int posJoueur[2], tPlateau copiePlateau, int*);

void zoom_in_out(char touche, int*, tPlateau plateau, char*, int*);

void enregistrerDeplacements(tTabDeplacement t, int nb, char fic[]);

void memoire_deplacement(char *val_seul, char *val_caisse, tTabDeplacement deplacementPlateau, bool *deplSeul, bool *deplCaisse, int*);


/**
 * @brief Fonction principale permettant d'afficher et jouer au sokoban
 * @return EXIT_SUCCESS : arret normal du programme
 */

int main(){ 

	char touche;
	char niveau[26];
	int posJoueur[2];

	char valCaisse;
	char valSeul;
	
	bool abandon;
	bool deplSeul;
	bool deplCaisse;

    char verifie;
    char nomFichier[26];

    int x1, x2, y1, y2, nbrCoups, zoom;
    x1 = 0;
    x2 = 0;
    y1 = 0;
    y2 = 0;

    nbrCoups = 0;

    abandon = false;

	zoom = 1; 

    touche = '\0';

    tPlateau plateau;
    tPlateau copiePlateau;
	tTabDeplacement deplacement;

    printf("Saisissez un niveau \n");
    scanf("%s", niveau);
    strcat(niveau, ".sok");

    charger_partie(plateau, niveau);
    charger_partie(copiePlateau,niveau);
    affiche_entete(niveau, &nbrCoups);
    affiche_plateau(plateau, &zoom);
    position_joueur(plateau, niveau, posJoueur);

    //boucle while pour jouer tant qu'on a pas gagne
    while( (gagne(copiePlateau, &nbrCoups) != true) && (abandon != true)){

        if(kbhit()){

            touche = getchar();

            deplacer(touche, posJoueur, plateau, niveau, &x1, &x2, &y1, &y2, &nbrCoups, copiePlateau, &zoom, 
					&valCaisse, &valSeul, deplacement, &deplSeul, &deplCaisse);

            if(touche == RECOMMANCE){

                recommancer(plateau, niveau, &nbrCoups, posJoueur, copiePlateau, &zoom);
            
            }
            
            if(touche == ABANDON){

                abandonner(copiePlateau);
                abandon = true;

                printf("Vous avez abandonner \n");
            }
	    zoom_in_out(touche, &zoom, plateau, niveau, &nbrCoups);
        }
    }
    //fin boucle while car victoire ou abandon
    
    system("clear");
    printf("Voulez vous enregistrez vos mouvement (\"o\" pour oui, \"n\" pour non) ? \n");
    scanf("%c", &verifie);

    if(verifie == OUI){

        printf("Donnez un nom de fichier (max 25 car) \n");
        scanf("%s", nomFichier);
        strcat(nomFichier, ".txt");

        enregistrer_partie(deplacement,nomFichier);

    }

    return EXIT_SUCCESS;
}

/**
 * @brief procedure qui permet d'afficher l'entete du jeux en fonction du nom de niveau et les nimbres de coups 
 * @param niveau de type caracteres, E/S : recois le nom de niveau a afficher
 * @param nbrcoups de type entier, E/S : recois le nombre de coups a afficher
 * @return pas de valeur mais un affichage du nom de niveau et le nombre de coups
 */

void affiche_entete(char *niveau, int *nbrCoups){

    system("clear");
    printf("%s \n\n", niveau);
    printf("Z pour aller vers le haut \n");
    printf("S pour aller vers le bas \n");
    printf("Q pour aller vers la gauche \n");
    printf("D pour aller vers la droite \n");
    printf("X pour abandonner \n");
    printf("R pour recommencer \n");
    printf("votre nombre de coups: %d \n\n", *nbrCoups);

}

/**
 * @brief procedure permettant d'afficher le niveau
 * @param plateau de type caractere , Entree : recois le plateau a chargé
 * @return pas de valeur mais un affichage du niveau
 */


void affiche_plateau(tPlateau plateau, int *zoom){

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


void zoom_in_out( char touche, int *zoom, tPlateau plateau, char *niveau, int *nbrCoups){
	if ( touche == ZOOM_IN  && (*zoom) < 3){
		(*zoom)++;
		system("clear");
		affiche_entete(niveau, nbrCoups);
		affiche_plateau(plateau,zoom);
	}
	else if(touche == ZOOM_OUT && (*zoom) > 1){
		(*zoom)--;
		system("clear");
		affiche_entete(niveau, nbrCoups);
		affiche_plateau(plateau,zoom);
	}
}

/**
 * @brief procedure permettant de verifier les touches et faires les action desirés 
 * en affichant le nouveau plateau avec la nouvelle position du joueur
 * @param touche de type chare, Entree : recoit la touche appuyé
 * @param posJoueur de type entier, E/S : recoit la position du joueur modifié
 * @param fichier de type char, E/S : recoit le fichier qui est le niveau a chargé
 * @param x1 de type entier , E/S : recoit la ligne a verifier pour se depalcer
 * @param x2 de type entier , E/S : recoit la ligne a verfirer pour deplacer les caisses
 * @param y1 de type entier , E/S : recoit la colonne a verifier pour se deplacer
 * @param y2 de type emtier , E/S : recoit la colonne a verifier pour deplacer les caisses
 * @param nbrCoups de type emtier , E/S : initalise un compteurs de coups
 * @param copiePlateau de type caracteres , E/S : recoit le plateau a affiche
 */


void deplacer(char touche, int posJoueur[2], tPlateau plateau, char fichier[], int *x1, int *x2, int *y1, int *y2, 
		int *nbrCoups, tPlateau copiePlateau, int *zoom, char *val_seul, char *val_caisse, tTabDeplacement deplacement, bool *deplSeul, bool *deplCaisse){

    *x1 = 0;
    *x2 = 0;
    *y1 = 0;
    *y2 = 0;


    if(touche == HAUT){

        system("clear");
        affiche_entete(fichier, nbrCoups);
        remplace_car(plateau, copiePlateau,posJoueur);
        haut(copiePlateau, posJoueur, x1, x2, y1, y2, zoom, val_caisse,val_seul, deplacement, deplSeul,deplCaisse, nbrCoups);


    }
    else if(touche == BAS) {

        system("clear");
        affiche_entete(fichier, nbrCoups);
        remplace_car(plateau, copiePlateau,posJoueur);
        bas(copiePlateau, posJoueur, x1, x2, y1, y2, zoom, val_caisse, val_seul, deplacement, deplSeul, deplCaisse, nbrCoups);
    
    }
    else if(touche == GAUCHE) {

            
        system("clear");
        affiche_entete(fichier, nbrCoups);
        remplace_car(plateau, copiePlateau,posJoueur);
        gauche(copiePlateau, posJoueur, x1, x2, y1, y2, zoom, val_caisse,val_seul, deplacement, deplSeul, deplCaisse, nbrCoups); 

    }
    else if(touche == DROITE) {

              
        system("clear");
        affiche_entete(fichier, nbrCoups);
        remplace_car(plateau, copiePlateau,posJoueur);
        droite(copiePlateau, posJoueur, x1, x2, y1, y2, zoom, val_caisse, val_seul, deplacement, deplSeul, deplCaisse, nbrCoups);
    }

    if(touche == HAUT || touche == BAS || touche == DROITE || touche == GAUCHE){

        (*nbrCoups)++;
    }
    
}

/**
 * @brief parcours le tableau pour trouver la position du joueur
 * @param plateau de type caracteres , E/S : recoit le plateau a parcourir
 * @param fichier de type caracteres , E/S : recoit le niveau a charge
 * @param posJoueur de type entier , E/S : recoit la position du joueur
 */
void position_joueur(tPlateau plateau, char fichier[], int posJoueur[2]){

    for(int x = 0 ; x < TAILLE ; x++){
        for(int y = 0 ; y < TAILLE ; y++){
            if(plateau[x][y] == PERSO){
                posJoueur[0] = x;
                posJoueur[1] = y;
                
            }
        }
    }
}

/**
 * @brief procedure qui permet d'abandonner et d'enregisré si voulu la partie
 * @param copiePlateau de type caracteres, E/S : recoit le plateau qui a été modifié
 */
void abandonner(tPlateau copiePlateau){
    
    char niveau[26];
    char verife;

    system("clear");
    printf("Voulez vous enregistrez (\"o\" pour oui, \"n\" pour non) ? \n");
    scanf("%c", &verife);

    if(verife == OUI){

        printf("Donnew un nom de fichier (max 25 car) \n");
        scanf("%s", niveau);
        strcat(niveau, ".sok");

        enregistrer_partie(copiePlateau,niveau);

    }

}

/**
 * @brief Procedure modifiant la ligne a verfier
 * @param copiePlateau de type caracteres , E/S : recoit le plateau a modifie
 * @param posJoueur de type entier , E/S  : recoit la position du joueur a modifie
 * @param x1 de type entier , E/S : modifie la ligne a verifier pour se depalcer
 * @param x2 de type entier , E/S : modifie la ligne a verfirer pour deplacer les caisses
 * @param y1 de type entier , E/S : modifie la colonne a verifier pour se deplacer
 * @param y2 de type emtier , E/S : modifie la colonne a verifier pour deplacer les caisses
 */
void haut(tPlateau copiePlateau, int posJoueur[2], int *x1, int *x2, int *y1, int *y2, int *zoom,  
    char *val_seul, char *val_caisse,tTabDeplacement deplacement, bool *deplSeul, bool *deplCaisse, int *nbrCoups){

    *x1 = -1;
    *x2 = -2;
    *val_seul = SOKO_SEUL_HAUT;
    *val_caisse = SOKO_CAISSE_HAUT;
    
    new_plateau(posJoueur,copiePlateau, x1, x2, y1, y2, zoom, deplSeul, deplCaisse);
    memoire_deplacement(val_seul, val_caisse,deplacement, deplSeul, deplCaisse, nbrCoups);

}
/** 
 * @brief Procedure modifiant la ligne a verfier
 * @param copiePlateau de type caracteres , E/S : recoit le plateau a modifie
 * @param posJoueur de type entier , E/S  : recoit la position du joueur a modifie
 * @param x1 de type entier , E/S : modifie la ligne a verifier pour se depalcer
 * @param x2 de type entier , E/S : modifie la ligne a verfirer pour deplacer les caisses
 * @param y1 de type entier , E/S : modifie la colonne a verifier pour se deplacer
 * @param y2 de type emtier , E/S : modifie la colonne a verifier pour deplacer les caisses
*/
void bas(tPlateau copiePlateau, int posJoueur[2], int *x1, int *x2, int *y1, int *y2, int *zoom,
   char *val_seul, char *val_caisse, tTabDeplacement deplacement, bool *deplSeul, bool *deplCaisse, int *nbrCoups){

    *x1 = 1;
    *x2 = 2;
    *val_seul = SOKO_SEUL_BAS;
    *val_caisse = SOKO_CAISSE_BAS;

    new_plateau(posJoueur,copiePlateau, x1, x2, y1, y2, zoom, deplSeul, deplCaisse);
    memoire_deplacement(val_seul, val_caisse, deplacement, deplSeul, deplCaisse, nbrCoups);

}


/** 
 * @brief Procedure modifiant la colonne a verfier
 * @param copiePlateau de type caracteres , E/S : recoit le plateau a modifie
 * @param posJoueur de type entier , E/S  : recoit la position du joueur a modifie
 * @param x1 de type entier , E/S : modifie la ligne a verifier pour se depalcer
 * @param x2 de type entier , E/S : modifie la ligne a verfirer pour deplacer les caisses
 * @param y1 de type entier , E/S : modifie la colonne a verifier pour se deplacer
 * @param y2 de type emtier , E/S : modifie la colonne a verifier pour deplacer les caisses
*/

void droite(tPlateau copiePlateau, int posJoueur[2], int *x1, int *x2, int *y1, int *y2, int *zoom, 
    char *val_seul, char *val_caisse, tTabDeplacement deplacement, bool *deplSeul, bool *deplCaisse, int *nbrCoups){

    *y1 = 1;
    *y2 = 2;
    *val_seul = SOKO_SEUL_DROITE;
    *val_caisse = SOKO_CAISSE_DROITE;

    new_plateau(posJoueur,copiePlateau, x1, x2, y1, y2, zoom, deplSeul, deplCaisse);
    memoire_deplacement(val_seul, val_caisse, deplacement, deplSeul, deplCaisse, nbrCoups);
}


/** 
 * @brief Procedure modifiant la colonne a verfier
 * @param copiePlateau de type caracteres , E/S : recoit le plateau a modifie
 * @param posJoueur de type entier , E/S  : recoit la position du joueur a modifie
 * @param x1 de type entier , E/S : modifie la ligne a verifier pour se depalcer
 * @param x2 de type entier , E/S : modifie la ligne a verfirer pour deplacer les caisses
 * @param y1 de type entier , E/S : modifie la colonne a verifier pour se deplacer
 * @param y2 de type emtier , E/S : modifie la colonne a verifier pour deplacer les caisses
*/


void gauche(tPlateau copiePlateau, int posJoueur[2], int *x1, int *x2, int *y1, int *y2, int *zoom,
		char *val_seul, char *val_caisse,tTabDeplacement deplacement, bool *deplSeul, bool *deplCaisse, int *nbrCoups){

    *y1 = -1;
    *y2 = -2;
    *val_seul = SOKO_SEUL_GAUCHE;
    *val_caisse = SOKO_CAISSE_GAUCHE;

    new_plateau(posJoueur,copiePlateau, x1, x2, y1, y2, zoom, deplSeul, deplCaisse);
    memoire_deplacement(val_seul, val_caisse, deplacement, deplSeul, deplCaisse, nbrCoups);
}


/**
 * @brief procedure permettant de faire les verifications pour deplacer et si possible modifie le tabaleau en consequance
 * @param posJoueur de type entier , E/S : recoit la posiiton du joueur
 * @param copiePlateau de type char , E/S : recoit le plateau et renvoi le plateau modife
 * @param x1 de type entier , E/S : recoit la ligne a verifier pour se depalcer
 * @param x2 de type entier , E/S : recoit la ligne a verfirer pour deplacer les caisses
 * @param y1 de type entier , E/S : recoit la colonne a verifier pour se deplacer
 * @param y2 de type emtier , E/S : recoit la colonne a verifier pour deplacer les caisses     if(verife == OUI){

        printf("Donnew un nom de fichier (max 25 car) \n");
        scanf("%s", niveau);
        strcat(niveau, ".sok");

        enregistrer_partie(copiePlateau,niveau);

    }
 */

void new_plateau(int posJoueur[2], tPlateau copiePlateau, int *x1, int *x2, int *y1, int *y2, int *zoom, bool *deplSeul, bool *deplCaisse){
	
    if(copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] != MUR){

        if(copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] == VIDE){  //verfie si la colonne d'a cote est libre en partant de la pos du joueur

            copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] = PERSO; // si la condition est rempli ecris arobase

            *deplSeul = true;
            *deplCaisse = false;
        }
        else if(copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] == CIBLE){

            copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] = PERSO_SUR_CIBLE;

            *deplSeul = true;
            *deplCaisse = false;
        }
        else if((copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] == CAISSE) && (copiePlateau[posJoueur[0] + *x2][posJoueur[1] + *y2] == VIDE)){

            copiePlateau[posJoueur[0] + *x2][posJoueur[1] + *y2] = CAISSE;
            copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] = PERSO;

            *deplSeul = false;
            *deplCaisse = true;
            
        }
        else if((copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] == CAISSE) && (copiePlateau[posJoueur[0] + *x2][posJoueur[1] + *y2] == CIBLE)){

            copiePlateau[posJoueur[0] + *x2][posJoueur[1] + *y2] = CAISSE_SUR_CIBLE;
            copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] = PERSO;

            *deplSeul = false;
            *deplCaisse = true;
        }
        else if((copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] == CAISSE_SUR_CIBLE) && (copiePlateau[posJoueur[0] + *x2][posJoueur[1] + *y2] == CIBLE)){

            copiePlateau[posJoueur[0] + *x2][posJoueur[1] + *y2] = CAISSE_SUR_CIBLE;
            copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] = PERSO_SUR_CIBLE;

            *deplSeul = false;
            *deplCaisse = true;
        }
        else if((copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] == CAISSE_SUR_CIBLE) && (copiePlateau[posJoueur[0] + *x2][posJoueur[1] + *y2] == VIDE)){

            copiePlateau[posJoueur[0] + *x2][posJoueur[1] + *y2] = CAISSE;
            copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] = PERSO;

            *deplSeul = false;
            *deplCaisse = true;
        }

        posJoueur[0] = posJoueur[0] + *x1;
        posJoueur[1] = posJoueur[1] + *y1;
	}		
    else if (copiePlateau[posJoueur[0] + *x1][posJoueur[1] + *y1] != CAISSE){

        copiePlateau[posJoueur[0]][posJoueur[1]] = PERSO;

        posJoueur[0] = posJoueur[0];
        posJoueur[1] = posJoueur[1];

        *deplSeul = false;
        *deplCaisse = false;
    }

    affiche_plateau(copiePlateau, zoom);

}

/**
 * @brief procedure permettant de remplacer les caracteres si besoin
 * @param plateau de type caractere , E/S : redcoit le niveau chargé
 * @param copiePlateau de type caracteres , E/S : recoit la copie du plateau a modifer
 * @param posjoueur de type etier , E/S : recoit la position du joueur pour momdifier
 */

void remplace_car(tPlateau plateau, tPlateau copiePlateau, int posJoueur[2]){

    //supprime le perso de sa pos init
    if(plateau[posJoueur[0]][posJoueur[1]] == PERSO ){

        copiePlateau[posJoueur[0]][posJoueur[1]] = VIDE; 
    }

    //supprime le perso a sa nouvelle place
    else if(plateau[posJoueur[0]][posJoueur[1]] == VIDE){
        
        copiePlateau[posJoueur[0]][posJoueur[1]] = VIDE;
    }
    
    //remet les cibles
    else if(plateau[posJoueur[0]][posJoueur[1]] == CIBLE){

        copiePlateau[posJoueur[0]][posJoueur[1]] = CIBLE;
    }

    //supprime le perso quand il recupere la caisse pour la premiere fois
    else if(plateau[posJoueur[0]][posJoueur[1]] == CAISSE){

        copiePlateau[posJoueur[0]][posJoueur[1]] = VIDE;
    }
  
}

void memoire_deplacement(char *val_seul, char *val_caisse, tTabDeplacement deplacementPlateau, bool *deplSeul, bool *deplCaisse, int *nbrCoups){
    

    if( (*deplSeul == true) && (*deplCaisse == false)){

        deplacementPlateau[ *nbrCoups] = *val_seul;

    }
    else if( (*deplSeul == false) && (*deplCaisse == true)){

        deplacementPlateau[*nbrCoups] = *val_caisse;
        
    }
    printf("%c \n", deplacementPlateau[ *nbrCoups]);

}








/**
 * @brief fonction boolenne qui retourne faux tant qu'il y'a des cibles ou le perso sur cible
 * @param plateau de type caracteres , E/S : recoit le plateau pour verfifier si il reste des cibles
 * @return Faux tant qu'il y'a des cibles et Vrai quand il n'y en a plus
 */

bool gagne(tPlateau plateau, int *nbrCoups){
    
    for(int x = 0 ; x < TAILLE ; x++){
        for(int y = 0; y < TAILLE ; y++){
            if(plateau[x][y] == CIBLE || plateau[x][y] == PERSO_SUR_CIBLE){

                return false;
                
            }
        }
    }
    printf("vous avez gagne en %d coups", *nbrCoups);
    return true;
    
    
}

/**
 * @brief procedure qui recommance la partie lancer si besoin
 * @param plateau de type caracteres , E/S : recoit le plateau a recharge
 * @param fichier de type caracteres , E/S : niveau choisi
 * @param nbrCoups de type entier , E/S : reinitialise le nbrcoups 
 * @param posJoueur de type entier, E/S : reinitialise la position du joueur
 * @param copiPlateau de type caracteres , E/S : recharge le plateau a modifie
 */

void recommancer(tPlateau plateau, char fichier[], int *nbrCoups, int posJoueur[2], tPlateau copiePlateau, int *zoom){
    
    char verife;

    *nbrCoups = 0;

    system("clear");
    printf("Voulez vous recommancer (\"o\" pour oui, \"n\" pour non) ? \n");
    scanf("%c", &verife);

    if(verife == OUI){

        system("clear");
        charger_partie(plateau, fichier);
        charger_partie(copiePlateau,fichier);
        affiche_entete(fichier, nbrCoups);
        affiche_plateau(plateau, zoom);
        position_joueur(plateau, fichier, posJoueur);
    }
    

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


void enregistrerDeplacements(tTabDeplacement t, int nb, char fic[]){
    FILE * f;

    f = fopen(fic, "w");
    fwrite(t,sizeof(char), nb, f);
    fclose(f);
}
