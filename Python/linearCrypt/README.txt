Implémentation de chiffrement/déchiffrement linéaire.

Ce chiffrement utilise une fonction affine de type y = mx+p.
Bien que m et p puissent être choisis, leur pgcd doit être égal à 1 afin de pouvoir déchiffrer.

# Exemple :
    fonction : 5x+6
    réferentiel (ref) : "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZéèçàù0123456789!#$%&'( )*+,-./:;<=>?@[\]^_`{|}~"
    mod : 99
    texte à chiffrer : mars
    cipher : 9g]|
    texte déchiffré : mars

# Explications :

soit y ≡ mx+p[mod]
'mod' étant la taille du réferentiel choisi, dans le cas de l'alphabet minuscule ou majuscule vaut 26.

# Chiffrement :
Pour chiffrer 'm' avec la fonction linéaire 5x+6 :
'm' est à l'index 12 si on part de a valant 0, on prend donc x = 12;

    (E) : y ≡ 5*12+6[26]
    (E) : y ≡ 66[26]
    (E) : y = 14

La lettre à l'index 14 est 'o';
Après application de ce chiffrement linéaire, 'm' est remplacé par 'o'.

# Déchiffrement :
Pour déchiffrer 'o' avec la fonction linéaire 5x+6 :
'o' est à l'index 14 si on part de a valant 0, on prend donc y = 14;

    (E) : y ≡ 5x+6[26]                  (E2) : y ≡ mx+p[mod]
    (E) : y-6 ≡ 5x[26]                  (E2) : y-p ≡ mx[mod]
    (E) : 21(y-6) ≡ 21*5x[26]           (E2) : pow(m,-1,mod)(y-p) ≡ pow(m,-1,mod)mx[mod]
    21 est l'inverse modulo de 5 
    par 26, on peut le déterminer 
    en python avec pow(5,-1,26)
    (E) : 21(y-6) ≡ 105x[26]            (E2) : pow(m,-1,mod)(y-p) ≡ pow(m,-1,mod)mx[mod]
    (E) : 21(y-6) ≡ x[26]               (E2) : pow(m,-1,mod)(y-p) ≡ pow(m,-1,mod)mx[mod]
    (E) : x ≡ 21y-126[26]               (E2) : x = ((pow(m,-1,mod)*(y-p))%mod)/((pow(m,-1,mod)*m)%mod)
    
    Après avoir déterminé la formule de déchiffrement, on peut l'appliquer :
    x ≡ 21*14-126[26]
    x ≡ 168[26]
    x = 12

La lettre à l'index 12 est 'm';
Après application de ce déchiffrement linéaire, on retrouve bien la lettre d'origine.

# Optimisation :
    De plus, on remarque que x = ((v*(y-p))%mod)/((v*m)%mod) comme démontré dans (E2), avec :
    m : coefficient directeur
    p : ordonnée à l'origine
    mod : taille du réferentiel (26 dans l'exemple précédent)
    v : inverse modulo de m par mod (déterminable avec pow(m,-1,mod)
    y : index de la lettre à déchiffrer

    On peut donc procéder plus rapidement pour le déchiffrement précédent :
        soit y = 14, m = 5, p = 6, mod = 26, v = pow(5,-1,26) = 21;
        donc x = ((21*(14-6))%26)/((21*5)%26)
        donc x = 12
