#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAILLE 12
typedef char t_Plateau[TAILLE][TAILLE];

void charger_partie(t_Plateau plateau, char fichier[]);
void enregistrer_partie(t_Plateau plateau, char fichier[]);
void afficher_plateau(t_Plateau plateau);
void afficher_entete(char fichier[]);
void deplacer(t_Plateau plateau, char fichier[], int *x, int *y);
void coord_jeu(t_Plateau plateau, int *x, int *y);
int kbhit();

int nombreDeplacement = 0;

int main()
{
    t_Plateau plateau;
    char fichier[11];
    int x, y;
    printf("Saisie le nom du Jeu : \n");
    scanf("%s", fichier);
    charger_partie(plateau, fichier);
    while (TAILLE != 10)
    {
        deplacer(plateau, fichier, &x, &y);
    }
    return 0;
}

void afficher_plateau(t_Plateau plateau)
{
    for (int i = 0; i < TAILLE; i++)
    {
        printf("\t\t");
        for (int j = 0; j < TAILLE; j++)
        {
            printf("%c", plateau[i][j]);
        }
        printf("\n");
    }
}

void afficher_entete(char fichier[])
{
    system("clear");
    printf("\tNom de la partie %s\n", fichier);
    printf("\n");
    printf("\t z pour avancer\n");
    printf("\t q pour aller a gauche\n");
    printf("\t s pour reculer\n");
    printf("\t d pour aller a droite\n");
    printf("\t x pour abandonner\n");
    printf("\t r pour recommencer\n");
    printf("\n");
    printf("\tNombre de déplacements \n\t        effectué : %d", nombreDeplacement);
    printf("\n");
}

void coord_jeu(t_Plateau plateau, int *x, int *y)
{
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (plateau[i][j] == '@')
            {
                *x = i;
                *y = j;
            }
        }
    }
}

void deplacer(t_Plateau plateau, char fichier[], int *x, int *y)
{
    char touche = '\0';
    if (kbhit())
    {
        system("clear");
        if (nombreDeplacement == 0)
        {
            coord_jeu(plateau, x, y);
        }
        int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
        touche = getchar();
        printf("%d", touche);
        if (touche == 'z')
        {
            x1 = -1;
            x2 = -2;
        }
        else if (touche == 's')
        {
            x1 = 1;
            x2 = 2;
        }
        else if (touche == 'q')
        {
            y1 = -1;
            y2 = -2;
        }
        else if (touche == 'd')
        {
            y1 = 1;
            y2 = 2;
        }
        if (plateau[*x + x1][*y + y1] == ' ' || plateau[*x + x1][*y + y1] == '.')
        {
            plateau[*x + x1][*y + y1] = '@';
            plateau[*x][*y] = ' ';
            nombreDeplacement++;
            *x += x1;
            *y += y1;
        }
        else if (plateau[*x + x1][*y + y1] == '$' && plateau[*x + x2][*y + y2] == ' ')
        {
            plateau[*x + x2][*y + y2] = '$';
            plateau[*x + x1][*y + y1] = '@';
            plateau[*x][*y] = ' ';
            nombreDeplacement++;
            *x += x1; 
            *y += y1;
        }
        else if (plateau[*x + x1][*y + y1] == '$' && plateau[*x + x2][*y + y2] == '.')
        {
            plateau[*x + x2][*y + y2] = '*';
            plateau[*x + x1][*y + y1] = '@';
            plateau[*x][*y] = ' ';
            nombreDeplacement++;
            *x += x1;
            *y += y1;
        }

        afficher_entete(fichier);
        afficher_plateau(plateau);
    }
}

void gagner(t_Plateau plateau)
{
}

void charger_partie(t_Plateau plateau, char fichier[])
{
    FILE *f;
    char finDeLigne;
    f = fopen(fichier, "r");
    if (f == NULL)
    {
        printf("ERREUR SUR FICHIER");
        exit(EXIT_FAILURE);
    }
    else
    {
        for (int ligne = 0; ligne < TAILLE; ligne++)
        {
            for (int colonne = 0; colonne < TAILLE; colonne++)
            {
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}

void enregistrer_partie(t_Plateau plateau, char fichier[])
{
    FILE *f;
    char finDeLigne = '\n';

    f = fopen(fichier, "w");
    for (int ligne = 0; ligne < TAILLE; ligne++)
    {
        for (int colonne = 0; colonne < TAILLE; colonne++)
        {
            fwrite(&plateau[ligne][colonne], sizeof(char), 1, f);
        }
        fwrite(&finDeLigne, sizeof(char), 1, f);
    }
    fclose(f);
}

int kbhit()
{
    // la fonction retourne :
    // 1 si un caractere est present
    // 0 si pas de caractere présent
    int unCaractere = 0;
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

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    }
    return unCaractere;
}

