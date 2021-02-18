def TakeOrders(instruct):
    commands_dict = {"-t":"", "-l":3, "-n":20, "-p":1}
    for command in commands_dict.keys():
        if command in instruct:
            x = ''
            pos_init = instruct.index(command)+3
            for c in instruct[pos_init:]:
                if c != " ":
                    x += c
                else:
                    if x.isnumeric():
                        x = int(x)
                    commands_dict[command] = x
                    break
    return commands_dict


def analyse(path,length_min=3):
    to_exclude = [';','.',':','=',',','+','-','>','<','*','/','^','?','@','%','!','{','}','~','\\','[',']','|','\n']
    mpos = {}
    pos = 1
    m = ''
    observe = False
    p_open = False
    with open(path, "r") as fichier:
        for line in fichier:
            if "Proof" in line:
                observe = True
                continue
            elif "Qed" in line or "Admitted" in line or "Defined" in line:
                observe = False
            if "(*" in line:
                d = line.index("(*")
            if "*)" in line:
                f = line.index("*)")+2
            if observe == True:
                if d and f:
                    line = line[:d] + line[f:]
                elif f:
                    line = line[:d]
                elif d:
                    line = line[f:]
                for c in line:
                    if c == "(":
                        p_open = True
                    elif c == ")":
                        p_open = False
                    if p_open == False:
                        for exclude in to_exclude:
                            c = c.replace(exclude," ")
                    if c != " " or c == " " and p_open == True:
                        m += c
                    else:
                        if m != '' and len(m)>=length_min:
                            mpos[pos] = m
                            pos += 1
                        m = ''
            d = ''
            f = ''
    return mpos


def Sequencing(dictio,p=1):
    sequenced_dictio = {}
    for pos in dictio.keys():
        seq = ""
        hold = True
        for x in range(pos,pos+p):
            try:
                seq += dictio[x] + ' '
            except KeyError:
                hold = False
        if hold:
            if seq in sequenced_dictio.keys():
                sequenced_dictio[seq] += 1
            else:
                sequenced_dictio[seq] = 1
    return sequenced_dictio


def FindMax(dictio,nb=20):
    tmax = len(dictio.items())
    full_dictio = []
    t = 1
    V = True
    while t <= nb and V:
        n = [k  for (k, val) in dictio.items() if val == max(dictio.values())]
        for x in n:
            if t > nb or t == tmax:
                V = False
                break
            t += 1
            full_dictio.append("{0}: {1}".format(x,dictio[x]))
            del dictio[x]
    return full_dictio


commands = TakeOrders(str(input(">>> ")) + " ")
print("\n".join(FindMax(Sequencing(analyse(commands["-t"],commands["-l"]),commands["-p"]),commands["-n"])))
