Convertisseur de bases pour nombres entiers et décimaux.
Codage VA+S, C1 et C2 supportés pour conversion vers binaire.
Permet d'isoler le motif périodique (entre parenthèses) si partie décimale infinie.

6 paramètres :  - nb (nombre à convertir, type str car support de caractères comme nombre; nécessaire),
                - baseSrc (base du nombre à convertir, type int; nécessaire),
                - baseDest (base cible du nombre à convertir, type int; nécessaire),
                - codage (codage si base cible binaire et résultat négatif, type str; optionnel, par défaut pas de codage),
                - spacing (espacement des chiffres/carctères du résultat, type bool; optionnel, par défaut True),
                - splittable (si codage spécifique séléctionné et appliqué, ajoute un caractère '|' suivie du nombre de bits 
                  sur lequel est codé le résultat; optionnel, par défaut False)
                  
Exemples d'utilisation :

print(ConvBase("49",10,3))                          --> 1211
print(ConvBase("-12",3,10))                         --> -0005
print(ConvBase("-c.3",45,4))                        --> -0230,0(10 3)00
print(ConvBase("-28",10,2,))                        --> -0001 1100
print(ConvBase("-28",10,2,spacing=False))           --> -00011100
print(ConvBase("-28",10,2,"c2",splittable=True))    --> 1110 0100|8
