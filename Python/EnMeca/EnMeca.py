import os
import cv2
import numpy as np
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter.filedialog import askopenfilename
from PIL import ImageTk, Image



WinP = tk.Tk()
WinP.title('EnMeca')

g                       = 9.81
x1p                     = 0
x1m                     = 0
y1                      = 0
p                       = 0
im_nb                   = 0
yh                      = 0
ti                      = []
list_img                = []
coords_x                = []
coords_y                = []
Ec                      = []
Epp                     = []
Em                      = []

masse       = tk.StringVar()
filename    = tk.StringVar()
scale       = tk.StringVar()
hmer        = tk.StringVar()
x1_scale    = tk.IntVar()
x2_scale    = tk.IntVar()
ips         = tk.IntVar()
ips.set(1)

FILETYPE = [("video files", ".mp4")]



def to_frames(videoPath,ips):
    #convertit video en images et détermine ¤t
    global ti,list_img
    vidcap = cv2.VideoCapture(videoPath)
    width = vidcap.get(3)
    height = vidcap.get(4)
    canevas.config(width=width,height=height)
    def getFrame(sec):
        vidcap.set(cv2.CAP_PROP_POS_MSEC,sec*1000)
        hasFrames,image = vidcap.read()
        if hasFrames:
            cv2.imwrite("image"+str(count)+".jpg", image)     # save frame as JPG file
            list_img.append(count)
        return hasFrames
    sec = 0
    count=1
    success = getFrame(sec)
    while success:
        count = count + 1
        sec = sec + 1/ips
        sec = round(sec, 2)
        ti.append(sec)
        success = getFrame(sec)



def track(event):
    #définit position cible sur position curseur 
    xm, ym = event.x, event.y
    canevas.coords(b, xm,ym,xm,ym+15)
    canevas.coords(d, xm,ym,xm+15,ym)
    canevas.coords(h, xm,ym,xm,ym-15)
    canevas.coords(g, xm,ym,xm-15,ym)
    label_pos.config(text="x : {0}     y : {1}".format(xm,canevas.winfo_reqheight()-ym))



def video_import():
    #Import vidéo (.mp4) puis appel fonction to_frames()
    filename.set(askopenfilename(filetypes=FILETYPE))
    to_frames(filename.get(),ips.get())
    canevas.bind("<Button-1>", getpos)
    next_img()



def next_img():
    #fais passer images jusqu'à la dernière, puis appel fonction calc_v() pour calcul données
    global im_nb,x_scale
    if im_nb < len(ti):
        im_nb += 1
        im = Image.open("image{0}.jpg".format(im_nb))
        canevas.image = ImageTk.PhotoImage(im)
        canevas.create_image(0, 0, image=canevas.image, anchor='nw')
        label_decompte.config(text="{0}/{1}".format(im_nb, len(list_img)))
        canevas.tag_raise(b)
        canevas.tag_raise(h)
        canevas.tag_raise(g)
        canevas.tag_raise(d)
        try:
            canevas.tag_raise(x_scale)
            canevas.tag_raise(line0)
        except NameError:
            pass
    else:
        canevas.unbind("<Button-1>")
        calc_v()
        



def getpos(event):
    #sauvegarde position curseur pour coordonnées du système
    global coords_x, coords_y,x1p,x1m
    x2p = event.x
    y2p = event.y
    taille_reelle = float(scale.get())
    taille_img    = abs(x1_scale.get()-x2_scale.get())
    r             = taille_reelle/taille_img
    if len(coords_x) == 0:
        x1p = x2p
    x2m = abs(x2p-x1p)*r+x1m
    y2m = (yh-y2p)*r+float(hmer.get())
    coords_x.append(x2m)
    coords_y.append(y2m)
    x1p = x2p
    x1m = x2m
    next_img()



def revert_bind():
    #bascule liaison des évènements du bouton gauche de la souris
    canevas.unbind("<Button-1>")
    canevas.bind("<Button-1>", set_scale)



def set_scale(event):
    #définit échelle de distance
    global p,x1_scale,y1,x2_scale
    if p==0:
        x1_scale.set(event.x)
        y1 = event.y
        p += 1
    else:
        global x_scale
        x2_scale.set(event.x)
        x_scale = canevas.create_line(x1_scale.get(),y1,x2_scale.get(),y1,
                                      fill="red",width=5,arrow=tk.BOTH,arrowshape=(5,0,10))
        p = 0
        canevas.unbind("<Button-1>")
        canevas.bind("<Button-1>", set_h)



def set_h(event):
    #définit hauteur à la mer
    global yh
    yh = event.y
    line0 = canevas.create_line(0,yh,canevas.winfo_reqwidth(),yh,fill='red',dash=(4,4))
    canevas.unbind("<Button-1>")
    canevas.bind("<Button-1>", getpos)



def calc_v():
    #calcul Epp, Ec, Em
    global coords_x, coords_y,Epp,ti
    t             = 1/ips.get()
    for y in coords_y:
        E_pp(y)
    coords_xn = coords_x[2:]
    coords_yn = coords_y[2:]
    coords_x  = iter(coords_x)
    coords_y  = iter(coords_y)
    coords_xn = iter(coords_xn)
    coords_yn = iter(coords_yn)
    for (x1, x2, y1, y2) in zip(coords_x, coords_xn, coords_y, coords_yn):
        vx = (x2-x1)/(2*t)
        vy = (y2-y1)/(2*t)
        vi = np.sqrt(vx**2+vy**2)
        E_c(vi)
    Epp = Epp[1:-1]
    for (c,pp) in zip(Ec,Epp):
        em = c+pp
        Em.append(em)
    ti = ti[1:-1]
    graph()



def E_pp(h):
    #calcul Epp
    global Epp
    epp = float(masse.get())*g*h
    Epp.append(epp)



def E_c(v):
    #calcul Ec
    global Ec
    ec = 1/2*float(masse.get())*v**2
    Ec.append(ec)



def graph():
    #trace graph
    global coords_x, coords_y, coords_xn, coords_yn
    c, = plt.plot(ti,Ec)
    c.set_label("Ec")
    pp, = plt.plot(ti,Epp)
    pp.set_label("Epp")
    m, = plt.plot(ti,Em)
    m.set_label("Em")
    plt.xlabel("temps (s)", fontsize=15)
    plt.ylabel("Energie (J)", fontsize=15)
    plt.legend()
    plt.show()
    coords_x = []
    coords_y = []
    coords_xn = []
    coords_yn = []
    for nb in list_img:
        os.remove("image{0}.jpg".format(nb))



label_masse     = tk.Label(WinP, text=' masse (kg):').grid(column=1, row=1, sticky=tk.NW)
label_ips       = tk.Label(WinP, text=' images par seconde :').grid(column=1, row=3, sticky=tk.NW)
label_scale     = tk.Label(WinP, text=' échelle (m): ').grid(column=1, row=5,sticky=tk.NW)
label_hmer      = tk.Label(WinP, text=' hauteur à la mer (m): ').grid(column=1, row=7, sticky=tk.NW)

entry_masse = tk.Entry(WinP, width=15, textvariable=masse)
entry_masse.grid(column=1,row=2,sticky=tk.N)
entry_ips = tk.Entry(WinP, width=15, textvariable=ips)
entry_ips.grid(column=1,row=4,sticky=tk.N)
entry_scale = tk.Entry(WinP, width=15, textvariable=scale)
entry_scale.grid(column=1,row=6,sticky=tk.N)
entry_hmer = tk.Entry(WinP, width=15, textvariable=hmer)
entry_hmer.grid(column=1,row=8,sticky=tk.N)

label_instruct = tk.Label(WinP, text="1/ entrez valeurs\n2/ bouton vidéo\n3/ bouton échelle :\n-2 premiers click\npour échelle\n-dernier click pour\nhauteur à la mer\n4/ pointer position\ndu système")
label_instruct.grid(column=1,row=10,rowspan=45)

label_decompte = tk.Label(WinP, text="0/0")
label_decompte.grid(column=1, row=56)

button_getvid = tk.Button(WinP, text="Vidéo", command=video_import).grid(column=1,row=59,sticky=tk.EW)
button_scale  = tk.Button(WinP, text="échelle", command=revert_bind).grid(column=1,row=60,sticky=tk.EW)

label_pos = tk.Label(WinP, text="x :   y : ")
label_pos.grid(column=2, row=61, sticky=tk.W)

canevas = tk.Canvas(WinP, width = 1000, height = 500, bg = "white", cursor="none")
canevas.grid(column=2, row=1, rowspan=60, sticky=tk.E)

b = canevas.create_line((canevas.winfo_reqwidth()/2),(canevas.winfo_reqheight()/2),
                        (canevas.winfo_reqwidth()/2),(canevas.winfo_reqheight()/2+15),fill="red")
d = canevas.create_line((canevas.winfo_reqwidth()/2),(canevas.winfo_reqheight()/2),
                        (canevas.winfo_reqwidth()/2+15),(canevas.winfo_reqheight()/2),fill="red")
h = canevas.create_line((canevas.winfo_reqwidth()/2),(canevas.winfo_reqheight()/2),
                        (canevas.winfo_reqwidth()/2),(canevas.winfo_reqheight()/2-15),fill="red")
g = canevas.create_line((canevas.winfo_reqwidth()/2),(canevas.winfo_reqheight()/2),
                        (canevas.winfo_reqwidth()/2-15),(canevas.winfo_reqheight()/2),fill="red")


canevas.bind("<Motion>", track)

WinP.mainloop()
