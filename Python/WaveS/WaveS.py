import numpy as np
import tkinter as tk
#éléments graphiques (lignes, cercles, ...)
from matplotlib.lines import Line2D
from matplotlib.figure import Figure
from matplotlib.patches import Ellipse
import matplotlib.style as mpl_style
#utilisé pour animer le graph
from matplotlib.animation import FuncAnimation
#permet l'intégration du graphe matplotlib dans un widget tkinter
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg



class WaveS:
    
    #accueil (choix de la taille de la fenêtre puis lancement)
    def __init__(self,app):
        self.app = app
        self.param = tk.IntVar()
        self.param1 = tk.Radiobutton(self.app, variable=self.param, value=1, text='mode 1                          (1712x800)')
        self.param1.pack()
        self.param2 = tk.Radiobutton(self.app, variable=self.param, value=2, text='mode 2                          (1412x720)')
        self.param2.pack()
        self.run_param = tk.Button(self.app,text='Launch',command=self.affectParam)
        self.run_param.pack(fill=tk.X)
        self.param1.invoke()
        self.wigets = self.app.winfo_children()
        self.app.update_idletasks()
        self.app.geometry('+{0}+{1}'.format(int((self.app.winfo_screenwidth()-self.app.winfo_width())/2*(1/5)),
                          int((self.app.winfo_screenheight()-self.app.winfo_height())/2*(1/5))))

    def affectParam(self):
        if self.param.get() == 1:
            self.size_graph = (15,4)
            self.pos_txt1 = 54
            self.pos_txt2 = 18
            self.pointer_ry = 1.4
        else:
            self.size_graph = (12,3.6)
            self.pos_txt1 = 52
            self.pos_txt2 = 17.7
            self.pointer_ry = 1.2
        for widget in self.wigets:
            widget.destroy()
        self.launch()


    def launch(self):
        self.alignement = 'center'
        ##################################
        #paramètres de la fonction
        self.v = 2
        self.A = 5
        self.T = 10
        self.L = self.T*self.v
        ##################################
        #vitesse de défilement du graph (self.vv)
        self.vv = 0.02
        self.ig = 0
        self.yA = 0
        self.xA = []
        self.yB = 0
        self.xB = []
        #affectation des sliders pour changer dynamiquement les paramètres de la fonction
        self.slide_A = tk.Scale(self.app,orient=tk.HORIZONTAL,from_=1,to_=100,label='A :',command=self.affectNewValues)
        self.slide_A.grid(row=0,column=1)
        self.slide_A.set(self.A*10)
        self.label_A = tk.Label(self.app)
        self.label_A.grid(row=1,column=1)
        self.slide_T = tk.Scale(self.app,orient=tk.HORIZONTAL,from_=1,to_=100,label='T :',command=self.affectNewValues)
        self.slide_T.grid(row=0,column=2)
        self.slide_T.set(self.T*10)
        self.label_T = tk.Label(self.app)
        self.label_T.grid(row=1,column=2)
        self.slide_vv = tk.Scale(self.app,orient=tk.HORIZONTAL,from_=1,to_=20,label='vitesse de défilement :',command=self.affectNewValues)
        self.slide_vv.grid(row=2,column=1,columnspan=2,sticky=tk.EW)
        self.slide_vv.set(5)
        self.btn_stop = tk.Button(self.app,text='STOP',command=self.switch_anim)
        self.btn_stop.grid(row=3,column=1,columnspan=2,sticky=tk.EW)
        self.mes_btn = tk.Button(self.app,text='Mesures',command=self.arrows)
        self.mes_btn.grid(row=4,column=1,columnspan=2,sticky=tk.EW)
        # création widget console-like pour affichage des valeurs
        self.display = tk.Text(self.app,width=20)
        self.display.grid(row=5,column=1,columnspan=2,rowspan=45,sticky=tk.NSEW)
        self.display.insert(tk.END,">>> WaveS 1.3 (2020)\n")
        self.display["state"]='disabled'
        #création des graphes matplotlib (vides)
        self.fig1 = Figure(figsize=self.size_graph,dpi=100)
        self.fig2 = Figure(figsize=self.size_graph,dpi=100)
        self.graph1 = self.fig1.add_subplot(111)
        self.graph2 = self.fig2.add_subplot(111)
        #liaison des graphes matplotlib aux widgets tkinter
        self.graphA = FigureCanvasTkAgg(self.fig1, master=self.app)
        self.graphB = FigureCanvasTkAgg(self.fig2, master=self.app)
        #intégration des graphes matplotlib aux widgets tkinter
        self.canvasA = self.graphA.get_tk_widget()
        self.canvasB = self.graphB.get_tk_widget()
        #positionnement des widget tkinter contennant les graphes matplotlib
        self.canvasA.grid(row=0,column=0,rowspan=25)
        self.canvasB.grid(row=25,column=0,rowspan=25)
        ########################################
        #génération des points à rendre sur les graphes
        self.x = np.linspace(0,60,600)
        self.t = np.linspace(0,20,600)
        ########################################
        #création du widget "rond" simulant une balle et son mouvement
        self.pointer = Ellipse((30,0),1,self.pointer_ry,color='k')
        self.graph1.add_artist(self.pointer)
        #génération des lignes sur les points passés en paramètres (self.x et self.t) 
        #d'après les fonctions (self.fx et self.ft) et leur paramètres (self.A, self.L et self.T)
        #en précisant leurs épaisseur ('lw' => line width)
        self.l1, = self.graph1.plot(self.x,self.fx(self.A,self.L),color='r',lw=2)
        self.l2, = self.graph2.plot(self.t,self.ft(self.A,self.T),color='k',lw=2)
        self.def_lim()
        self.app.resizable(0,0)
        #lancement de l'animation sur fig1 avec la fonction self.animate pour l'actualisation des données
        self.anim = FuncAnimation(self.fig1, self.animate, frames=600, interval=25, init_func=self.init_blit, blit=True)
        
    #####################################################################################################################
    def fx(self,A,L,i=0): return A*np.sin(((2*np.pi)/L)*(self.x+i*self.vv))

    def ft(self,A,T): return A*np.cos(((2*np.pi)/T)*self.t)
    #####################################################################################################################
    
    def def_lim(self):
        #repaint des éléments graphiques
        font_x = 15
        font_y = 20
        lim_y = (-10,10)
        self.graph1.set(xlim=(0,60),ylim=lim_y)
        self.graph1.set_xlabel('X (cm)',fontsize=font_x)
        self.graph1.set_ylabel('élongation de\nla corde',fontsize=font_y)
        self.graph2.set(xlim=(0,20),ylim=lim_y)
        self.graph2.set_xlabel('t (s)',fontsize=font_x)
        self.graph2.set_ylabel("élongation d'un point\nde la corde",fontsize=font_y)

    def affectNewValues(self,*args):
        #affectation des nouvelles valeurs aux paramètres de fonctions
        self.graph1.clear()
        self.graph2.clear()
        self.A = round(0.1*self.slide_A.get(),1)
        self.T = round(0.1*self.slide_T.get(),1)
        self.vv = round(0.02 *self.slide_vv.get(),3)
        self.L = self.T*self.v
        self.def_lim()
        self.label_A.config(text=self.A)
        self.label_T.config(text=self.T)

    def switch_anim(self):
        #système 'marche/arrêt' pour visualiser les données statiques et/ou effectuer les mesures
        if self.btn_stop["text"] == 'STOP':
            self.anim.event_source.stop()
            self.slide_A.configure(state='disabled')
            self.slide_T.configure(state='disabled')
            self.slide_vv.configure(state='disabled')
            self.btn_stop.configure(text='START')
        else:
            try:
                self.fig1.canvas.mpl_disconnect(self.g11)
                self.fig1.canvas.mpl_disconnect(self.g12)
                self.fig2.canvas.mpl_disconnect(self.g21)
                self.fig2.canvas.mpl_disconnect(self.g22)
                for widget in self.annot:
                    widget.remove()
            except:
                pass
            self.slide_A.configure(state='normal')
            self.slide_T.configure(state='normal')
            self.slide_vv.configure(state='normal')
            self.mes_btn.configure(state='normal')
            self.anim.event_source.start()
            self.btn_stop.configure(text='STOP')

    def animate(self,i):
        self.ig = i
        cy = self.fx(self.A,self.L,i)
        self.l1.set_ydata(cy)
        self.pointer.set_center((max(self.x)/2,cy[int(len(self.x)/2)]))
        self.l2.set_ydata(self.ft(self.A,self.T))
        return self.l1,self.pointer,self.l2,

    def init_blit(self):
        #génération des points initiaux
        self.l1.set_ydata(np.ma.array(self.x, mask=True))
        self.l2.set_ydata(np.ma.array(self.t, mask=True))
        self.pointer.set_center((30, 0))
        return self.l1,self.l2,self.pointer

    def arrows(self):
        #gestion des prises de mesures suivant l'action de la souris sur les graphes
        #et rendu visuel des distances mesurées
        self.annot = []
        self.anim.event_source.stop()
        self.btn_stop.configure(text='START')
        self.mes_btn.configure(state='disabled')
        self.slide_A.configure(state='disabled')
        self.slide_T.configure(state='disabled')
        self.slide_vv.configure(state='disabled')
        self.ly1 = self.graph1.axvline(color='b')
        self.annot.append(self.ly1)
        self.ly2 = self.graph2.axvline(color='b')
        self.annot.append(self.ly2)
        self.txt1 = self.graph1.text(self.pos_txt1,9,'x=  y= ')
        self.annot.append(self.txt1)
        self.txt2 = self.graph2.text(self.pos_txt2,9,'x=  y= ')
        self.annot.append(self.txt2)
        self.disp1 = self.graph1.text(30,8.5,'', horizontalalignment=self.alignement,verticalalignment=self.alignement,fontsize=20,color='b')
        self.annot.append(self.disp1)
        self.disp2 = self.graph2.text(10,8.5,'', horizontalalignment=self.alignement,verticalalignment=self.alignement,fontsize=20,color='b')
        self.annot.append(self.disp2)
        self.tr1 = Line2D((0,0),(0,0),color='b',linewidth=1,marker='|',markersize=15,markeredgewidth=2)
        self.annot.append(self.tr1)
        self.tr2 = Line2D((0,0),(0,0),color='b',linewidth=1,marker='|',markersize=15,markeredgewidth=2)
        self.annot.append(self.tr2)
        self.graph1.add_line(self.tr1)
        self.graph2.add_line(self.tr2)
        cy = self.fx(self.A,self.L,self.ig)
        self.l1, = self.graph1.plot(self.x,cy,color='r',lw=2)
        self.l2, = self.graph2.plot(self.t,self.ft(self.A,self.T),color='k',lw=2)
        self.def_lim()
        self.fig1.canvas.draw()
        self.fig2.canvas.draw()
        #gestion des évènements souris (survol et pressions sur les graphes)
        self.g11 = self.fig1.canvas.mpl_connect('motion_notify_event', self.track1)
        self.g12 = self.fig1.canvas.mpl_connect('button_press_event', self.peak1)
        self.g21 = self.fig2.canvas.mpl_connect('motion_notify_event', self.track2)
        self.g22 = self.fig2.canvas.mpl_connect('button_press_event', self.peak2)

    def track1(self,event):
        if not event.inaxes:
            return
        x, y = event.xdata, event.ydata
        self.ly1.set_xdata(x)
        self.txt1.set_text('x= %1.2f, y= %1.2f' % (x, y))
        self.fig1.canvas.draw()

    def track2(self,event):
        if not event.inaxes:
            return
        x, y = event.xdata, event.ydata
        self.ly2.set_xdata(x)
        self.txt2.set_text('x= %1.2f, y= %1.2f' % (x, y))
        self.fig2.canvas.draw()

    def getpeak(self,xN,yN,x,y):
        xN.append(x)
        yN = y
        if len(xN)<=1:
            return xN,yN,False
        else:
            return xN,yN,True

    def peak1(self,event):
        x, y = event.xdata, event.ydata
        if not event.inaxes:
            return
        self.xA, self.yA, r = self.getpeak(self.xA,self.yA,x,y)
        if r == True:
            d = abs(round(self.xA[0]-self.xA[1],2))
            self.tr1.set_data(tuple(self.xA),(self.yA,self.yA))
            self.disp1.set_text('{0} cm'.format(d))
            self.display.configure(state='normal')
            self.display.insert(tk.END,"\n‎Δd = {0} m".format(round(d/100,3)))
            self.display.configure(state='disabled')
            self.fig1.canvas.draw()
            self.xA = []

    def peak2(self,event):
        x, y = event.xdata, event.ydata
        if not event.inaxes:
            return
        self.xB, self.yB, r = self.getpeak(self.xB,self.yB,x,y)
        if r == True:
            d = abs(round(self.xB[0]-self.xB[1],2))
            self.tr2.set_data(tuple(self.xB),(self.yB,self.yB))
            self.disp2.set_text('{0} s'.format(d))
            self.display.configure(state='normal')
            self.display.insert(tk.END,"\n‎Δt = {0} s".format(round(d,3)))
            self.display.configure(state='disabled')
            self.fig2.canvas.draw()
            self.xB = []


def main():
    app = tk.Tk()
    app.wm_title("WaveS")
    mpl_style.use('seaborn-whitegrid')
    WaveS(app)
    app.mainloop()

if __name__ == '__main__':
    main()
