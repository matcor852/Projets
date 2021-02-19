Générateur de questions de conversion style qcm, enregistré au format csv (séparateur : ';').

6 paramètres :  - start_questions (début de numérotation des questions, type int; nécessaire),
                - nb_questions (nombre de questions à générer, type int; nécessaire),
                - borne_min (valeur minimale que peut prendre la réponse [avant conversion]; type int, base 10; nécessaire),
                - borne_max (valeur maximale que peut prendre la réponse [avant conversion]; type int, base 10; nécessaire),
                - float_possibility (possibilité de décimales dans question/propositions de réponses; type str [y/n], nécessaire),
                - etendue (interval de valeurs possibles pour les fausses propositions, de [(nb_ref-etendue)/2;(nb_ref+etendue)/2]
                  avec nb_ref étant la réponse attendue, type int; nécessaire)


Exemples d'utilisation : 
--------------------------------------------------------------------------------------------------------------------------------
début questions : 0
nb de questions : 5
borne min : -5
borne max : 5
possiblité nombres décimaux (y/n) : y
étendue des réponses : 10

num_question;num_theme;validation;question;reponseOK;reponse2;reponse3;reponse4
0;1;ok;Que vaut 0x-0003,F000 en base 10 ?;-0003,9375;-0005,7500;-0006,9375;-0008,9687 5000
1;1;ok;Que vaut 0005 en base 2 ?;0000 0101;0000 0010;0000 1000;0000 1010
2;1;ok;Que vaut 0x-0004,8000 en base 2 ?;-0000 0100,1000 0000;-0000 0110,1110 0000;-0000 1011,1111 0000;-0000 0111,1111 1000
3;1;ok;Que vaut -0004 en base 2 VA+S sur 8 bits ?;1000 0100;0000 0001;1000 0011;1000 0111
4;1;ok;Que vaut -0007,8750 en base 2 ?;-0000 0111,1110 0000;-0000 1111,1111 1000;-0000 1010,1100 0000;-0000 0101,1100 0000
--------------------------------------------------------------------------------------------------------------------------------
début questions : 12
nb de questions : 5
borne min : -50
borne max : 200
possiblité nombres décimaux (y/n) : n
étendue des réponses : 25

num_question;num_theme;validation;question;reponseOK;reponse2;reponse3;reponse4
12;1;ok;Que vaut 0x006B en base 10 ?;0107;0108;0097;0104
13;1;ok;Que vaut 0x-0013 en base 10 ?;-0019;-0011;-0010;-0023
14;1;ok;Que vaut 0b0111 1111 en base 16 ?;007F;007C;007E;0076
15;1;ok;Que vaut 0x00BB en base 2 ?;1011 1011;1011 1100;1100 0000;1100 0111
16;1;ok;Que vaut 0022 en base 2 ?;0001 0110;0001 1001;0000 1111;0010 0000
--------------------------------------------------------------------------------------------------------------------------------
