f = open("scripts_list.txt", "w")

iterations_1 = [1_000]
threads_1 = [1,2,4,8,12,16,24]

iterations_2 = [10, 50, 100, 500, 1_000]
threads_2 = [1,4,8,12,16]

iterations_3 = [30, 150, 300, 1500, 3_000]
threads_3 = [1,4,8,12,16]

iterations_4 = [1_000]
threads_4 = [1,2,4,8,12]
lists_4 = [4,8,16]

yields = ["n", "i", "d", "il", "dl"]
locks = ["n", "s", "m"]

for l in locks[1:]:
    for thread in threads_1:
        for it in iterations_1:
            cmd = "-lab2_list"
            if (l != "n"):
                cmd += (" --sync=" + l)
            cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2b_list.csv\n")
            f.write(cmd)

for thread in threads_2:
    for it in iterations_2:
        cmd = "-lab2_list"
        cmd += " --yield=id --lists=4"
        cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2b_list.csv\n")
        f.write(cmd)

for l in locks[1:]:
    for thread in threads_3:
        for it in iterations_3:
            cmd = "-lab2_list"
            cmd += " --yield=id --lists=4"
            if (l != "n"):
                cmd += (" --sync=" + l)
            cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2b_list.csv\n")
            f.write(cmd)

for l in locks[1:]:
    for thread in threads_4:
        for li in lists_4:
            for it in iterations_4:
                cmd = "-lab2_list"
                if (l != "n"):
                    cmd += (" --sync=" + l)
                cmd += (" --lists=" + str(li))
                cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2b_list.csv\n")
                f.write(cmd)

# for it in iterations_1:
#     cmd = "./lab2_list"
#     cmd += (" --iterations=" + str(it) + " --threads=1 >> lab2b_list.csv\n")
#     f.write(cmd)

# # for y in yields:
#     for it in iterations_3:
#         for thread in threads:
#             cmd = "./lab2_list"
#             if (y != "n"):
#                 cmd += (" --yield=" + y)
#             cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2b_list.csv\n")
#             f.write(cmd)
# for y in yields:
#     for l in locks[1:]:
#         it = 32
#         thread = 12
#         cmd = "./lab2_list"
#         if (y != "n"):
#             cmd += (" --yield=" + y)
#         if (l != "n"):
#             cmd += (" --sync=" + l)
#         cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2b_list.csv\n")
#         f.write(cmd)
# for thread in threads_2:
#     for l in locks[1:]:
#         it = 1000
#         cmd = "./lab2_list"
#         if (l != "n"):
#             cmd += (" --sync=" + l)
#         cmd += (" --iterations=" + str(it) + " --threads=" + str(thread) + " >> lab2b_list.csv\n")
#         f.write(cmd)
f.close()