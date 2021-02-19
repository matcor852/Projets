import string, secrets

def password_generation(password_length = 12, nbChiffres = 3, nbCaractSp = 3):
    if((nbChiffres + nbCaractSp) >= password_length):
        print("erreur : taille mdp insuffisante pour paramètres choisis")
        return
    password_found = False
    punct = string.punctuation
    alphabet = string.ascii_letters + string.digits + punct
    while password_found == False:
        password = ''.join(secrets.choice(alphabet) for i in range(password_length))
        if (any(c.islower() for c in password) and any(c.isupper() for c in password)
                and sum(c.isdigit() for c in password) >= 3
                and sum(c in punct for c in password) >= 3):
            password_found = True
    return password


nbCaractères            = 12
nbChiffres              = 10
nbCaractèresSpéciaux    = 10


print(password_generation(nbCaractères, nbChiffres, nbCaractèresSpéciaux))
