        int x1,x2,y1,y2;

        x1 = 0; 
        x2 = 0;
        y1 = 0;
        y2 = 0;

        y1 = 1;
        y2 = 2;

        if (plateau[pos_joueur[0] + x1][pos_joueur[1] + y1] == ' ' || plateau[pos_joueur[0] + x1][pos_joueur[1] + y1] == '.'){  //verfie si la colonne d'a cote est libre en partant de la pos du joueur

            plateau[pos_joueur[0] + x1][pos_joueur[1] + y1] = '@'; // si la condition est rempli ecris arobase
            plateau[pos_joueur[0]][pos_joueur[1]] = ' '; //supprime l'arobase a l'ancienne place

            pos_joueur[0] = x1;
            pos_joueur[1] = y1;  
            printf("%d", pos_joueur[0]);
            printf("%d", pos_joueur[1]);
        }