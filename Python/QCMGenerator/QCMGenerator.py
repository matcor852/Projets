import csv
import random
from classConvBase import ConvBase


class QCMG:
    
    def __init__(self):
        with open('qcm.csv', 'w', newline='') as f:
                writer = csv.writer(f, delimiter=";")
                writer.writerow(("num_question","num_theme","validation","question","reponseOK","reponse2","reponse3","reponse4"))
        self.start_questions = int(input("début questions : "))
        self.nb_questions = int(input("nb de questions : "))
        self.borne_min = int(input("borne min : "))
        self.borne_max = int(input("borne max : "))
        self.float_possiblity = str(input("possiblité nombres décimaux (y/n) : "))
        self.etendue = int(input("étendue des réponses : "))
        self.bases = {2: "0b", 10: "", 16: "0x"}
        self.codages = ["C1","C2","VA+S"]
        self.decimal = ["y","n"]
        self.assembly()
        
    def assembly(self):
        for i in range(self.start_questions,self.start_questions+self.nb_questions):
            self.nb_already_in = []
            self.dictio_random_rep = {"reponse2":"", "reponse3":"", "reponse4":""}
            if self.float_possiblity == "y":
                self.v_float = random.choice(self.decimal)
            else:
                self.v_float = "n"
            base_init, base_dest = random.sample(self.bases.keys(),2)
            nb_selected = self.true_rep()
            if base_dest == 2 and nb_selected < 0 and self.v_float == "n":
                codage = random.choice(self.codages)
            else:
                codage = ""
            nb_ref = ConvBase(str(nb_selected),10,base_init)
            reponseOK, nb_bits = str(ConvBase(str(nb_ref),base_init,base_dest,codage,True,True)).split("|")
            while "(" in str(reponseOK):
                nb_selected = self.true_rep()
                if base_dest == 2 and nb_selected < 0 and self.v_float == "n":
                    codage = random.choice(self.codages)
                else:
                    codage = ""
                nb_ref = ConvBase(str(nb_selected),10,base_init)
                reponseOK, nb_bits = str(ConvBase(str(nb_ref),base_init,base_dest,codage,True,True)).split("|")
            self.nb_already_in.append(str(nb_selected))
            if nb_bits:
                nb_bits = " sur "+nb_bits+" bits"
            for rep in self.dictio_random_rep.keys():
                witness, picked_rep = self.random_rep(nb_selected,base_init,base_dest,codage)
                while witness in self.nb_already_in or "(" in picked_rep:
                    #print("retrying for q{0} {1}...".format(i,rep))
                    witness, picked_rep = self.random_rep(nb_selected,base_init,base_dest,codage)
                self.nb_already_in.append(str(witness))
                self.dictio_random_rep[rep] = str(picked_rep)
            if codage:
                codage = " "+codage
            #print("reponseOk : ",reponseOK)
            with open('qcm.csv', 'a', newline='') as f:
                writer = csv.writer(f, delimiter=";")
                writer.writerow((i,1,"ok","Que vaut {0}{1} en base {2}{3}{4} ?".format(self.bases[base_init],nb_ref,base_dest,codage,nb_bits),
                                 reponseOK,
                                 self.dictio_random_rep["reponse2"],
                                 self.dictio_random_rep["reponse3"],
                                 self.dictio_random_rep["reponse4"]))

    def true_rep(self):
        if self.v_float == "y":
            p_dec_temp = 2**(random.randint(-5,-1)) + (random.randint(-5,-2))
        else:
            p_dec_temp = 0
        nb_selected = random.randint(self.borne_min,self.borne_max) + p_dec_temp
        return nb_selected

    def random_rep(self,nb_ref,base_init,base_dest,codage):
        if self.v_float == "y":
            p_dec_temp = 2**(random.randint(-5,-1)) + (random.randint(-5,-2))
        else:
            p_dec_temp = 0
        witness = nb_temp = random.randint(int(nb_ref-self.etendue/2),int(nb_ref+self.etendue/2)) + p_dec_temp
        nb_temp = ConvBase(str(nb_temp),10,base_init)
        return str(witness), str(ConvBase(str(nb_temp),base_init,base_dest,codage))

QCMG()
