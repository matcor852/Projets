from math import pow

class ConvBase:
    
    def __init__(self,nb,baseSrc,baseDest,codage="",spacing=True,splittable=False):
        self.baseSrc = baseSrc
        self.baseDest = baseDest
        self.repr_approx = codage.upper()
        self.spacing = spacing
        self.splittable = splittable
        #suppression des séparateurs
        nb = nb.replace(",",".")
        nb = nb.replace(" ","")
        self.conversion(nb)
    
    def conversion(self,nb):
        try:#----------------------------------------------------   différencie partie entière (int) et décimale (float) du nombre
            p_1, p_2 = str(nb).split('.')
            p_2 = self.negTest(p_2)
            p_1 = self.negTest(p_1)
            self.testValeurs(p_1)
            self.testValeurs(p_2)
            nbDecimales = len(p_2)
            only_int = False
        except ValueError:
            p_1 = self.negTest(nb)
            self.testValeurs(nb)
            only_int = True
        if only_int:
            self.result = str("{0}".format(self.convBase(p_1)))
        else:
            self.result = str("{0}.{1}".format(self.convBase(p_1),self.convDec(p_2,nbDecimales,only_int)))
        self.toNBits()
        if self.baseDest == 2 and self.signe == "-" and only_int:
            dictio_repr = {"VA+S": self.toVAS, "C1": self.toC1, "C2": self.toC2, "": self.DoNothing}
            dictio_repr[self.repr_approx]()
        if self.repr_approx == "":
            self.nb_bits = ""
        self.__repr__()
        
    
    def __repr__(self):
        if self.splittable:
            return "{0}{1}|{2}".format(self.signe,self.result,self.nb_bits)
        else:
            return self.signe + self.result
    
    
    def DoNothing(self): return
    
    
    def toC2(self):
        nb = list(reversed((self.result)))
        tm = int(nb[0])
        if tm-1 < 0:
            retenue = True
            for i,char in enumerate(nb):
                if retenue:
                    if char == "0":
                        nb[i] = "1"
                    elif char == "1":
                        nb[i] = "0"
                        retenue = False
        else:
            nb[0] = "0"
        self.result = "".join(list(reversed((nb))))
        self.toC1()
    
    
    def toC1(self):
        self.result = self.result.translate(str.maketrans('10', '01'))
        self.toVAS()

    
    def toVAS(self):
        nb = list(self.result)
        nb[0] = "1"
        self.result = "".join(nb)
        self.signe = ""

    
    def toNBits(self):
        if self.baseDest == 2:
            multiple = 8
        else:
            multiple = 4
        try:
            dictio = {"p_int":"", "p_float":""}
            dictio["p_int"], dictio["p_float"] = str(self.result).split('.')
            p_int = False
        except ValueError:
            dictio = {"p_int":self.result}
            p_int = True
        for p in dictio.keys():
            length_nb = len(str(dictio[p]))
            if self.signe and self.baseDest == 2 and p == "p_int":
                length_nb += 1
            multiplier = 1
            while length_nb > multiple*multiplier:
                multiplier += 1
            nb_bits = multiple*multiplier
            nb_0 = ["0"] * (nb_bits - length_nb)
            if self.signe and self.baseDest == 2 and p == "p_int":
                nb_0.append("0")
            if p == "p_int":
                dictio[p] = "".join(nb_0 + list(map(str,dictio[p])))
                self.nb_bits = nb_bits
            else:
                dictio[p] = "".join(list(map(str,dictio[p])) + nb_0)
                self.nb_bits += nb_bits
        if p_int:
            self.result = dictio["p_int"]
        else:
            self.result = "{0}.{1}".format(dictio["p_int"],dictio["p_float"])
        if self.spacing:
            result = list(self.result)
            n = 0
            for i,j in enumerate(result):
                if n == 4:
                    if j == ".":
                        result[i] = ","
                    else:
                        result.insert(i," ")
                    n = -1
                n += 1
            self.result = "".join(list(map(str,result)))
    
    
    def negTest(self,nb):#----------------------------------------   gère signe du nombre
        m = "-"
        nb = list(str(nb))
        if m in nb:
            self.signe = m
            nb.remove(m)
        else:
            self.signe = ""
        return (''.join(nb))
    
    
    def testValeurs(self,nb):#-----------------  teste la validité des valeurs entrées
        nb = list(str(nb))
        v = ','
        if v in nb:
            print('\n\terreur: veuillez séparer la partie entière et décimale par un point, non une virgule')
            raise ValueError()
        if self.baseSrc < 2 or self.baseDest < 2:
            print('\n\terreur: base inférieure à 2 inexistante')
            raise ValueError()
        for i in list(nb):
            if self.convChiffreVersIntBase10(str(i)) > (self.baseSrc - 1):
                print(f'\n\terreur: chiffre supérieur ou égal à la base source dans le nombre : {i}')
                raise ValueError()
    
    
    def convBase(self,nombre):#----------------   convertit les entiers
        valeurDecimale = self.convTo10(nombre)
        if valeurDecimale == 0:
            nombreConverti = '0'
        else:
            nombreConverti = ""
        while valeurDecimale > 0:
            nombreConverti = self.convIntBase10VersChiffre(int(valeurDecimale % self.baseDest)) + nombreConverti
            valeurDecimale //= self.baseDest
        return nombreConverti
    
    
    def convDec(self,nombre,nbDecimales,only_int):#-------   convertit les décimales
        p_float = self.convTo10(nombre, only_int)
        p_fin = []
        factorList = []
        while p_float > 0:
            p_float = round(p_float*self.baseDest, nbDecimales)
            p_int, p_float = str(p_float).split(".")
            p_fin += self.convIntBase10VersChiffre(int(p_int))
            if int(p_float) in factorList:
                break
            factorList.append(int(p_float))
            p_float = float('0.' + str(p_float))
        if p_float != 0:
            p_fin.insert(-(len(factorList)), '(')
            p_fin.insert(len(p_fin), ')')
        return (''.join(map(str, p_fin)))
    
    
    def convChiffreVersIntBase10(self,chiffre):#------------------   convertit caractère en chiffre base 10
        if chiffre >= '0' and chiffre <= '9' :
            return int(chiffre)
        else:
            return ord(chiffre) - ord('A') + 10
    
    
    def convIntBase10VersChiffre(self,nombre):#-------------------   convertit chiffre base 10 en caractère
        if nombre in range(10) :
            return str(nombre)
        else:
            return chr(int(nombre) + ord('A') - 10)
    
    
    def convTo10(self,nombre,only_int=True):#---------   convertit nombre en base 10
        valeurDecimale = 0
        for i in range(len(nombre)):
            if only_int is True:
                e = len(nombre)-i-1
            else:
                e = -(i+1)
            valeurDecimale += self.convChiffreVersIntBase10(nombre[i]) * pow(self.baseSrc, e)
        return valeurDecimale
    
print(ConvBase("101000000",2,10))
