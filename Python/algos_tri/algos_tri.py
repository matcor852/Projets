import random
import time

def tri_insertion(tableau):    #comparaison à n et n+1 par tour
    t = list(tableau)          #constructeur copie
    j = 1
    while j < len(t):
        i = j-1
        k = t[j]
        while i>=0 and t[i]>k:
            t[i+1] = t[i]
            i -= 1
        t[i+1] = k
        j += 1
    return t


def tri_selection(tableau):   #comparaison à n jusqu'a nmax par tour
    t = list(tableau)         #constructeur copie
    i = 0
    while i < len(t):
        j = i + 1
        minus = i
        while j < len(t):
            if t[j] < t[minus]:
                minus = j
            j += 1
        if minus != i:
            t[i], t[minus] = t[minus], t[i]
        i += 1
    return t


def tri_monotonie(tableau):
    t = list(tableau)
    dicto = {True:[], False:[]}
    current = True
    while False if False not in (t[i]<=t[i+1] for i in range(len(t)-1)) else True:
        dicto[current].append(t.pop(0))
        while len(t):
            if dicto[current][-1] > t[0]:
                current = not current
            dicto[current].append(t.pop(0))
        t.clear()
        while len(dicto[True]) and len(dicto[False]):
            if dicto[current] > dicto[not current]:
                current = not current
            t.append(dicto[current].pop(0))
        if len(dicto[True]):
            t.extend(dicto[True])
            dicto[True].clear()
        else:
            t.extend(dicto[False])
            dicto[False].clear()
    return t
        

def tri_fusion_recursif(tableau):
    def divise(liste):
        l = liste[0:len(liste)//2+len(liste)%2]
        r = liste[len(liste)//2+len(liste)%2:]
        return l,r
    if len(tableau) == 1:
        return tableau
    tab = []
    l,r = divise(tableau)
    left = tri_fusion_recursif(l)
    right = tri_fusion_recursif(r)
    while len(left) and len(right):
        if left[0] <= right[0]:
            tab.append(left.pop(0))
        else:
            tab.append(right.pop(0))
    if len(left):
        tab.extend(left)
    else:
        tab.extend(right)
    return tab



class sorter:
    def __init__(self,tableau):
        tps1 = time.perf_counter()
        if len(tableau) < 7:
            print("by selection")
            self.v = tri_selection(tableau)
        elif len(tableau) <169:
            print("by insertion")
            self.v = tri_insertion(tableau)
        else:
            print("by fusion")
            self.v = tri_fusion_recursif(tableau)
        tps2 = time.perf_counter()
        print(tps2 - tps1, "\n")
    
    
    def __repr__(self):
        return repr(self.v)
            
        
    def tri_insertion(self,tableau):
        t = list(tableau)
        j = 1
        while j < len(t):
            i = j-1
            k = t[j]
            while i>=0 and t[i]>k:
                t[i+1] = t[i]
                i -= 1
            t[i+1] = k
            j += 1
        return t
    
    
    def tri_selection(self,tableau):
        t = list(tableau)
        i = 0
        while i < len(t):
            j = i + 1
            minus = i
            while j < len(t):
                if t[j] < t[minus]:
                    minus = j
                j += 1
            if minus != i:
                t[i], t[minus] = t[minus], t[i]
            i += 1
        return t
    
    
    def tri_fusion_recursif(self,tableau):
        def divise(liste):
            l = liste[0:len(liste)//2+len(liste)%2]
            r = liste[len(liste)//2+len(liste)%2:]
            return l,r
        if len(tableau) == 1:
            return tableau
        tab = []
        l,r = divise(tableau)
        left = tri_fusion_recursif(l)
        right = tri_fusion_recursif(r)
        while len(left) != 0 and len(right) != 0:
            if left[0] <= right[0]:
                tab.append(left.pop(0))
            else:
                tab.append(right.pop(0))
        if len(left) > 0:
            tab.extend(left)
        else:
            tab.extend(right)
        return tab


print(sorter([random.randint(1,50) for i in range(30)]))
