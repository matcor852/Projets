import sys

def pgcd(a,b):
    while b!=0:
        a,b=b,a%b
    return a

def linearCrypt(m,x,p,mod):
    return (m*x+p)%mod

def linearDecrypt(m,y,p,mod):
    #v : inverse modulo de m par mod
    v = pow(m,-1,mod)
    return ((v*(y-p))%mod)/((v*m)%mod)


# y = mx+p
m = 5
p = 6
ref = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZéèçàù0123456789!#$%&'( )*+,-./:;<=>?@[\]^_`{|}~"
mod = len(ref)
#tout les caractères de 'plain' doivent être dans 'ref'
plain = "linear crypt"

if pgcd(m,mod) != 1:
    print("base is not invertible for the given modulus; pgcd of 'm' and 'mod' must equal 1.")
    sys.exit()


#crypt
cipher = ""
for char in plain:
    cipher += ref[linearCrypt(m,ref.index(char),p,mod)]
print("cipher : ", cipher)

#decrypt
plaintext = ""
for char in cipher:
    plaintext += ref[int(linearDecrypt(m,ref.index(char),p,mod))]
print("plaintext : ",plaintext)
