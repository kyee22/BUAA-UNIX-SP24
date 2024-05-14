
with open("1.txt", "r") as f:
    ls = f.readlines()
    for l in ls:
        print('printf(\"\\033[1;33m' + l[]+");", end='')