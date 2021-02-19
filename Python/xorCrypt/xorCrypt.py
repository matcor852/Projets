def toStr(bits):
    return ''.join(chr(int(''.join(x),2)) for x in zip(*[iter(bits)]*8))

def toBits(s):
    result = []
    for c in s:
        bits = bin(ord(c))[2:]
        bits = '00000000'[len(bits):]+bits
        result.extend([int(b) for b in bits])
    return result


def xor_encrypt(plaintext,key):
    data = toBits(plaintext)
    password = toBits(key)
    cipher = ""
    for dataBit, keyBit in zip(data, password):
        cipher += str(dataBit ^ keyBit)
    return cipher


plain = "murs"
key = "maisons"

cipher = xor_encrypt(plain,key)
print("cipher : ", cipher)

uncipher = xor_encrypt(toStr(cipher),key)
print("plaintext : ", toStr(uncipher))
