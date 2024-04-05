f = open("scripts.txt", "w")

iterations = [10, 20, 40, 80, 100, 1000, 10000, 100000]
threads = [1, 2, 4, 8, 12]
sync = ["n", "m", "s", "c"]

# for s in sync:
#     for i in range(2):
#         for it in iterations:
#             for thread in threads:
#                 cmd = "./lab2_add"
#                 if (s != "n"):
#                     cmd += (" --sync=" + s)
#                 if i:
#                     cmd += " --yield"
#                 cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2_add.csv\n")
#                 f.write(cmd)

for i in range(2):
    for it in iterations:
        for thread in threads:
            cmd = "./lab2_add"
            if i:
                cmd += " --yield"
            cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2_add.csv\n")
            f.write(cmd)

for thread in threads:
    for s in sync[1:]:
        cmd = "./lab2_add"
        
        cmd += " --yield"
        cmd += (" --sync=" + s)
        it = 0
        if (s == "m" or s == "c"):
            it = 10000
        else:
            it = 1000
        cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2_add.csv\n")
        f.write(cmd)

for thread in threads:
    for s in sync[1:]:
        cmd = "./lab2_add"
        cmd += (" --sync=" + s)
        it = 10000
        cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2_add.csv\n")
        f.write(cmd)

f.close()