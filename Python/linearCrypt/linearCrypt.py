import string, sys

def nbDigits(x):
    #return number of digits in x
    return len("%i"%x)

def pgcd(a,b):
    while b!=0:
        a,b=b,a%b
    return a

def linearCrypt(m,x,p,mod):
    return (m*x+p)%mod

def linearDecrypt(m,y,p,mod):
    #v : inverse modulo of m by mod
    v = pow(m,-1,mod)
    return ((v*(y-p))%mod)/((v*m)%mod)


m = 3
p = 6
plain = "linear crypt"

ref = string.ascii_letters + string.whitespace + string.digits + string.punctuation
mod = len(ref)
max_l = nbDigits(mod)

if pgcd(m,mod) != 1:
    print("base is not invertible for the given modulus; pgcd of 'm' and 'mod' must equal 1.")
    sys.exit()


#crypt
cipher = ""
for char in plain:
    cipher_char = linearCrypt(m,ref.index(char),p,mod)
    cipher += '0'*(max_l-nbDigits(cipher_char)) + str(cipher_char)
print("cipher : ", cipher)


#decrypt
plaintext = ""
for i in range(int(len(cipher)/max_l)):
    pos = i*max_l
    plaintext += ref[int(linearDecrypt(m,int(cipher[pos:pos+max_l]),p,mod))]
print("plaintext : ",plaintext)
