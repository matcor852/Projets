# ARNdecode.py
# AUTEUR : Matthieu CORREIA
# CLASSE : 1ère 5
# GROUPE : Groupe A
# DATE : 23/12/2019 - 05/01/2020
# LANGAGE : Python 3.x


import tkinter as tk
from tkinter import messagebox
import re
from tkinter.filedialog import askopenfilename, asksaveasfilename


class GifAnimatedLabel(tk.Label):
    #Animation adn .gif
    def __init__(self, master, filename, speed, *args, **kwargs):
        self.speed = speed
        self.frames = []
        i = 0
        while True:
            try:
                p = tk.PhotoImage(file=filename, format="gif - {}".format(i))
            except tk.TclError:
                break
            self.frames.append(p)
            i += 1

        super().__init__(master, image=self.frames[0], *args, **kwargs)
        self.frame_idx = 0
        self.num_frames = i
        self.after(self.speed, self._animate)

    def _animate(self):
        self.frame_idx = (self.frame_idx + 1) % self.num_frames
        self['image'] = self.frames[self.frame_idx]
        self.after(self.speed, self._animate)



app = tk.Tk()
app.title("ARNdecode")
app.geometry("800x450")
app.resizable(0,0)


origine_codon        = tk.IntVar()
origine_source       = tk.IntVar()
entry1               = tk.StringVar()
entry2               = tk.StringVar()
filename             = tk.StringVar()


codons_in_txt        = ''
liste_arn            = ''
break_file_opening   = False
break_file_transcrip = False


FILETYPE = [ ("text files", "*.txt") ]
to_erase = ['\n', ' ']
infos_Messagebox = """- Déterminer l'origine de votre entrée
- Si fichier .txt, insérer un fichier .txt dans le champ.
    Fichier valide si codons séparés par des virgules
    (ex. fichier valide : aug,UGG,uuc,...,uga)
- Si codon, entrer les codons un par un, insensible à la casse
- Observer l'acide aminé correspondant
- Suiver la création de la séquence d'acides aminés
- Enregistrer la séquence d'acides aminée dans un fichier .txt avec le boutton 'Enregistrer sous...'
- Remettre valeurs par défaut avec le boutton 'Réinitialiser'

        1er codon d'une séquence : aug/AUG (Methionine)
        dernier codon d'une séquence : uga/UAA/uag

                        ADN <=> ARN
                             A   =   U
                             C   =   G
                             G   =   C
                             T   =   A"""



def applyChanges(arn, entry):
    #applique changements après chaque entrée
    img_related = tk.PhotoImage(file='{0}.ppm'.format(arn))
    img_molecule.configure(image=img_related)
    img_molecule.image=img_related
    nom_molecule["text"]=arn
    if origine_source.get() == 1:
        codon_1["state"]='normal'
        if codon_1.get() != entry:
            codon_1.delete(0, tk.END)
            codon_1.insert(tk.END, entry)
        codon_1["state"]='disabled'
    codon_2.delete(0, tk.END)
    afficheur["state"]='normal'
    if arn == 'Methionine':
        afficheur.insert(tk.END, arn)
    else:
        afficheur.insert(tk.END, ".{0}".format(arn))
    if arn == 'Codon-STOP':
        afficheur.insert(tk.END, "\n")
    afficheur["state"]='disabled'



#Délimite séquences valides (commance par Methionine et finit par Codon-Stop)
def find_sequence(liste_arn, entry):
    tab_seq  = []
    nb_debut = liste_arn.count('Methionine')
    i = 0
    while i <= nb_debut-1:
        i += 1
        seq_temp = []
        fin = liste_arn.index('Codon-STOP')
        debut = liste_arn.index('Methionine')
        for pos, codon in enumerate(liste_arn, 0):
            if pos >= debut and pos <= fin:
                seq_temp.append(codon)
        del liste_arn[0:fin+1]
        if seq_temp:
            tab_seq.append(seq_temp)
    for sequence in tab_seq:
        for occur in sequence:
            applyChanges(occur, entry)



#Trouve acide aminé correspondant au codon
def transcrip(entry, taille_fichier=1):
    global break_file_transcrip, liste_arn
    arn = 'erreur'
    with open("codonsARN.txt", "r") as codons:
        for line in codons:
            if entry in line:
                liste_codons = line.split(',')
                arn = liste_codons[0]
                break
    if arn != 'erreur':
        if origine_source.get() == 1:
            applyChanges(arn, entry)
        else:
            liste_arn += ','
            liste_arn += str(arn)
            if liste_arn[0] == ',':
                liste_arn = liste_arn[1:]
            temp_arn = liste_arn
            liste_arn = liste_arn.split(',')
            if len(liste_arn) == taille_fichier:
                find_sequence(liste_arn, entry)
                liste_arn = ''
            else:
                liste_arn = temp_arn
    else:
        if origine_source.get() == 2:
            break_file_transcrip = True
            tk.messagebox.showwarning("Erreur ouverture", "Fichier invalide")
        else:
            tk.messagebox.showwarning("Valeur inconnue", "Ceci n'est pas un codon viable")


def ARNorADN(entry, taille_fichier=1):
    #Convertit ADN en ARN
    global codons_in_txt, break_file_opening, break_file_transcrip
    break_file_transcrip = False
    if len(entry) == 3:
        if origine_codon.get() == 2:
            codon = list(entry)
            entry = ''
            for p1 in codon:
                if 'A' in p1:
                    p1 = re.sub('A', 'U', p1)
                elif 'C' in p1:
                    p1 = re.sub('C', 'G', p1)
                elif 'G' in p1:
                    p1 = re.sub('G', 'C', p1)
                else:
                    p1 = re.sub('T', 'A', p1)
                entry += p1
        codons_in_txt += ','
        codons_in_txt += str(entry)
        if codons_in_txt[0] == ',':
            codons_in_txt = codons_in_txt[1:]
        if origine_source.get() == 1:
            codons_in_txt = ''
            transcrip(entry)
        if origine_source.get() == 2 and len(codons_in_txt) == (taille_fichier*3+taille_fichier-1):
            codons_in_txt = codons_in_txt.split(',')
            for occur in codons_in_txt:
                transcrip(occur, taille_fichier)
                if break_file_transcrip:
                    break
            codons_in_txt = ''
    else:
        if origine_source.get() == 2:
            break_file_opening = True
            tk.messagebox.showwarning("Erreur ouverture", "Fichier invalide")
        else:
            tk.messagebox.showwarning("Valeur inconnue", "Ceci n'est pas un codon viable")



def getEntry():
    #Obtient les entrées
    global codons_in_txt, break_file_opening
    if origine_source.get() == 1:
        if codon_1["state"] == 'normal':
            entry = entry1.get().upper()
            codon_1["state"]='disabled'
            codon_2["state"]='normal'
            codon_2.focus_set()
        else:
            entry = entry2.get().upper()
        ARNorADN(entry)
    else:
        break_file_opening = False
        try:
            with open(filename.get(), "r") as fichier:
                txt = fichier.read()
            for erase in to_erase:
                txt = re.sub(erase, '', txt)
            txt = txt.split(',')
            for codon in txt:
                ARNorADN(codon.upper(), len(txt))
                if break_file_opening:
                    codons_in_txt = ''
                    break
        except:
            tk.messagebox.showerror("Erreur", "Echec de l'ouverture")



def CODONorTXT():
    #Gère l'affichage et rendu visuel
    #après basculement Radiobuttons
    if origine_source.get() == 2:
        codon_1["state"]='normal'
        codon_2["state"]='normal'
        codon_1.delete(0, tk.END)
        codon_2.delete(0, tk.END)
        codon_1["state"]='disabled'
        codon_2["state"]='disabled'
        ouvrir_b["state"]='normal'
        ouvrir_e["state"]='normal'
        ouvrir_e.focus_set()
    else:
        codon_1["state"]='normal'
        codon_1.delete(0, tk.END)
        codon_1.focus_set()
        codon_2.delete(0, tk.END)
        codon_2["state"]='disabled'
        ouvrir_b["state"]='disabled'
        ouvrir_e["state"]='normal'
        ouvrir_e.delete(0, tk.END)
        ouvrir_e["state"]='disabled'



def set_filename_import():
    #Import fichier .txt puis traitement
    filename.set(askopenfilename(filetypes=FILETYPE))
    getEntry()



def savefile():
    #Enregistrement dans fichier .txt après traitement
    afficheur["state"]='normal'
    save_content = str(afficheur.get(0.0, tk.END))
    afficheur["state"]='disabled'
    try:
        filename_export = asksaveasfilename(filetypes=FILETYPE, defaultextension='.txt', initialdir='C:/Users/')
    except:
        filename_export = asksaveasfilename(filetypes=FILETYPE, defaultextension='.txt')
    try:
        with open(filename_export, "w") as write_file:
            write_file.write(save_content)
    except:
        tk.messagebox.showerror("Erreur", "Echec de l'enregistrement")



def infobox():
    #Instructions et conseils d'utilisation
    tk.messagebox.showinfo("Infos", infos_Messagebox)



def re_init():
    #Réinitialisation interface
    img_related = tk.PhotoImage(file='Codon-STOP.ppm')
    img_molecule.configure(image=img_related)
    img_molecule.image=img_related
    nom_molecule["text"]=''
    afficheur["state"]='normal'
    afficheur.delete(0.0, tk.END)
    afficheur["state"]='disabled'
    c_ARN.invoke()
    c_codon.invoke()
    codon_1.focus_set()



######################################  Création widgets et liaisons commandes  ##########################################
    
#création gif adn
left_partP = GifAnimatedLabel(app, filename="Z-DNA_orbit_animated.gif", speed=30).grid(column=0, row=0, rowspan=40)

#création widgets choix origine entrées
label_origine = tk.Label(app, text='origine :').grid(column=1, row=1, sticky=tk.W)

c_ARN = tk.Radiobutton(app, text="ARN", variable=origine_codon, value=1)
c_ARN.grid(column=2, row=1, sticky=tk.W)

c_ADN = tk.Radiobutton(app, text="ADN", variable=origine_codon, value=2).grid(column=3, row=1, sticky=tk.W)

c_codon = tk.Radiobutton(app, text="codon", variable=origine_source, value=1, command=CODONorTXT)
c_codon.grid(column=2, row=2, sticky=tk.W)

c_txt = tk.Radiobutton(app, text="fichier .txt", variable=origine_source, value=2, command=CODONorTXT).grid(column=3, row=2, sticky=tk.W)

#création widgets acquisition entrées
#codon_1 -> 1ère zone d'acquisition codon entré manuellement si c_codon coché
codon_1 = tk.Entry(app, width=4, textvariable=entry1)
codon_1.grid(column=2, row=15)

#codon_2 -> 2ème zone d'acquisition codon entré manuellement si c_codon coché
codon_2 = tk.Entry(app, width=4, textvariable=entry2)
codon_2.grid(column=3, row=15, sticky=tk.W)

#ouvrir_b -> bouton ouverture fichier .txt si c_txt coché
ouvrir_b = tk.Button(app, text="ouvrir", command=set_filename_import)
ouvrir_b.grid(column=4, row=12, sticky=tk.W)

#ouvrir_e -> zone d'entrée nom de fichier .txt à ouvrir si c_txt coché
ouvrir_e = tk.Entry(app, width=30, textvariable=filename)
ouvrir_e.grid(column=1, row=12, columnspan=3, sticky=tk.E)

#liaison widgets pour touche "Entrée" pressée -> appel de fonction getEntry()
codon_1.bind("<Return>", lambda x: getEntry())
codon_2.bind("<Return>", lambda x: getEntry())
ouvrir_e.bind("<Return>", lambda x: getEntry())



"""
++++++++++++++++++++++++++++++++++++
création widget zone d'affichage et ses scrollbars pour suivi dynamique 
------------------------------------    
"""

afficheur_scroll_X = tk.Scrollbar(app, orient='horizontal')
afficheur_scroll_X.grid(column=1, row=30, columnspan=8, sticky=tk.N+tk.W+tk.E)

afficheur_scroll_Y = tk.Scrollbar(app, orient='vertical')
afficheur_scroll_Y.grid(column=9, row=25, rowspan=4, sticky=tk.W+tk.S+tk.N)

afficheur = tk.Text(app, wrap=tk.NONE, state='disabled', width=50, height=5)
afficheur.grid(column=1, row=28, columnspan=8, sticky=tk.S+tk.W+tk.E)

#liaison scrollbars avec afficheur
afficheur.config(xscrollcommand=afficheur_scroll_X.set, yscrollcommand=afficheur_scroll_Y.set)
afficheur_scroll_X.config(command=afficheur.xview)
afficheur_scroll_Y.config(command=afficheur.yview)



#création widgets affichage dynamique molécule pour codon
molecule = tk.Frame(app, width=250, height=250)
molecule.grid_propagate(False)
molecule.grid(column=5, row=1, columnspan=5, rowspan=20)

nom_molecule = tk.Label(app)
nom_molecule.grid(column=5, row=0, columnspan=5)

img = tk.PhotoImage(file='Codon-STOP.ppm')
img_molecule = tk.Label(molecule, image=img)
img_molecule.grid()



#création widgets boutons infos + gestion
infos = tk.Button(app, text="Infos", command=infobox).grid(column=1, row=35, columnspan=1)
save = tk.Button(app, text="Enregistrer sous...", command=savefile).grid(column=8, row=35, columnspan=2)
reset = tk.Button(app, text="Réinitialiser", command=re_init).grid(column=6, row=35, columnspan=2)



re_init()
app.mainloop()
