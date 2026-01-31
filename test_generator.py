import random
import subprocess

def gen(n, input_file):
    with open(input_file, "w") as f:
        outgoing = [[] for _ in range(n)]
        ingoing = [[] for _ in range(n)]
        num_input = random.randint(1, min((n + 1) // 2, 10))
        num_output = random.randint(1, (n + 2) // 3)
        # 0 = input, 1 = output, 2 = and, 3 = or, 4 = not, 5 = xor
        types = [random.randint(0, 5) for _ in range(n)]
        if types.count(0) > 15:
            for i in range(n):
                if types[i] == 0:
                    types[i] = random.randint(2, 5)
                    if types.count(0) <= 15:
                        break
        for i in range(3 * n):
            a = random.randint(0, n - 1)
            b = random.randint(0, n - 1)
            if types[b] != 0 and types[a] != 1: 
                if types[b] == 4 and len(ingoing[b]) < 1:
                    outgoing[a].append(b)
                    ingoing[b].append(a)
        
        rand_order = list(range(n))
        random.shuffle(rand_order)
        type_names = 'INPUT OUTPUT AND OR NOT XOR'.split()
        f.write("{\n")
        for i in rand_order:
            f.write(" " * random.randint(0, 5) + "{" + " " * random.randint(0, 5) + "\n")
            f.write(" " * random.randint(0, 5) + f"Type={type_names[types[i]]}" + " " * random.randint(0, 5) + "\n")
            f.write(" " * random.randint(0, 5) + f"UniqueID={i}" + " " * random.randint(0, 5) + "\n")
            f.write(" " * random.randint(0, 5) + "Input=")
            for x in ingoing[i]:
                f.write(f"{x},")
            f.write(" " * random.randint(0, 5) + "\n")
            f.write(" " * random.randint(0, 5) + "Output=")
            for x in outgoing[i]:
                f.write(f"{x},")
            f.write(" " * random.randint(0, 5) + "\n")
            f.write(" " * random.randint(0, 5) + "}" + " " * random.randint(0, 5) + "\n")
        f.write("}\n")
    

for i in range(1, 11):
    gen(random.randint(1, 10 * i), f"{i}_in.txt")