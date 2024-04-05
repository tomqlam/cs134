f = open("scripts_list.txt", "w")

iterations_1 = [10, 100, 1000, 10000, 20000]
iterations_2 = [1, 10, 100, 1000]
iterations_3 = [1, 2, 4, 8, 16, 32]
threads = [2, 4, 8, 12]
threads_2 = [1, 2, 4, 8, 12, 16, 24]

yields = ["n", "i", "d", "il", "dl"]
locks = ["n", "s", "m"]

for it in iterations_1:
    cmd = "./lab2_list"
    cmd += (" --iterations=" + str(it) + " --threads=1 >> lab2_list.csv\n")
    f.write(cmd)

for y in yields:
    for it in iterations_3:
        for thread in threads:
            cmd = "./lab2_list"
            if (y != "n"):
                cmd += (" --yield=" + y)
            cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2_list.csv\n")
            f.write(cmd)
for y in yields:
    for l in locks[1:]:
        it = 32
        thread = 12
        cmd = "./lab2_list"
        if (y != "n"):
            cmd += (" --yield=" + y)
        if (l != "n"):
            cmd += (" --sync=" + l)
        cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2_list.csv\n")
        f.write(cmd)
for thread in threads_2:
    for l in locks[1:]:
        it = 1000
        cmd = "./lab2_list"
        if (l != "n"):
            cmd += (" --sync=" + l)
        cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2_list.csv\n")
        f.write(cmd)
f.close()