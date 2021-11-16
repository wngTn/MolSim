import getopt
import random
import sys

MAX_V = 1.0
MIN_V = -1.
MAX_COORD = 50.
MIN_COORD = -50.
MAX_MASS = 1.


def cs():
    # gauss distribution
    return round(random.gauss(0, 15), 2), \
           round(random.gauss(0, 15), 2), \
           round(random.gauss(0, 15), 2),
    # random with uniform distribution
    # return round(random.uniform(MIN_COORD, MAX_COORD), 2), \
    #       round(random.uniform(MIN_COORD, MAX_COORD), 2), \
    #       round(random.uniform(MIN_COORD, MAX_COORD), 2)


def vs():
    # maybe use gauss distribution here too
    return round(random.uniform(MIN_V, MAX_V), 3), \
           round(random.uniform(MIN_V, MAX_V), 3), \
           round(random.uniform(MIN_V, MAX_V), 3)


def m():
    # this gives us a lot of small values < 1.0 and mostly towards <= 0.3 but also a few outliers way heavier at +5.0
    r = random.paretovariate(3) - 1.0  # -1.0 bc pareto values always >= 1.0

    # random mass between 0 and 1
    # r = random.random()
    if r == 0.:
        return 0.0001
    return round(r, 3)


def generate_values(count):
    crds = []
    velos = []
    ms = []
    for i in range(0, count):
        crds.append(cs())
        velos.append(vs())
        ms.append(m())
    return crds, velos, ms


def get_input():
    file = input("enter the output filename (with file extension like .txt)\n")

    part_s = input("enter the amount of particles (max 128)\n")
    particle_count = min(128, int(part_s))

    return file, particle_count


def help():
    print('usage: python generate_input.py [-n <particle_count>] [-o <outputfile>]')


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    file = ''
    particle_count = 50
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hn:o:', ['help', 'output='])
    except getopt.GetoptError as err:
        print(err)
        help()
        sys.exit()
    for opt, arg in opts:
        if opt == '-h':
            help()
            sys.exit()
        elif opt == '-n':
            particle_count = int(arg)
        elif opt in ('-o', '--output'):
            file = arg

    if file == '':
        file = 'output.txt'

    # file, particle_count = get_input()
    coords, vels, masses = generate_values(particle_count)
    zipped = list(zip(coords, vels, masses))
    with open(file, "w") as f:
        f.write("# xyz coord    velocity    mass\n")
        f.write(str(particle_count) + "\n")
        f.writelines([f'{x} {y} {z}    {v1} {v2} {v3}    {m}\n' for ((x, y, z), (v1, v2, v3), m) in zipped])
