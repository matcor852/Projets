Analyseur de code en language Coq pour les occurences de mots comprises entre 'Proof' et 'Qed' ou 'Admitted' ou 'Defined'.
Ce qui est compris entre parenthèses est considéré comme un unique mot.

4 paramètres :  -t (chemin), 
                -l (longueur minimale des mots),
                -n (nombre d'occurences à afficher, décroissant), 
                -p (occurences de p mots successifs)
                
exemple avec le fichier fourni :

-t exCodeCoq.txt -l 5 -n 5 -p 3
(afficher le top 5 des occurences de 3 mots successifs de 5 caractères minimum du fichier exCodeCoq.txt)

ce qui affiche :

simpl rewrite simpl : 7
simpl destruct (leb x1 x2) : 4
destruct (leb x1 x2) simpl : 4
(leb x1 x2) simpl rewrite : 4
rewrite simpl simpl : 4
