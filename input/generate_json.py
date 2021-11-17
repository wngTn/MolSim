import json
import os
import getopt
import random
import sys

# points to input directory
ROOT_DIR = os.path.dirname(os.path.abspath(__file__))

# points to where we want to save the files
SAVE_DIR = os.path.join(ROOT_DIR, 'files')

output_sizes = {
    's': [3, 16, 32, 16],
    'small': [3, 16, 32, 16],
    'm': [6, 20, 48, 20],
    'medium': [6, 20, 48, 20],
    'l': [9, 32, 64, 43],
    'large': [9, 32, 64, 43]
}


def write_json_benchmark(size):
    if size not in ('s', 'small', 'm', 'medium', 'l', 'large'):
        print("Didn't specify correct size, uses default (small)")

    data = {'shapes': []}
    for x in range(output_sizes.get(size)[0]):
        # if x % 2 == 0:
            data['shapes'].append({
                'type': 'cuboid',
                'pos': [x * 10, x * 20, x * 15],
                'vel': [0.01 * x, 0.08 * x, 0.16 * x],
                'N': [output_sizes.get(size)[1], output_sizes.get(size)[2], output_sizes.get(size)[3]],
                'distance': 1.1225,
                'mass': 1.0,
                'brownianFactor': 0.1,
                'brownianDIM': 3
            })
        # else:
        #     data['shapes'].append({
        #         'type': 'sphere',
        #         'pos': [x * 1., x * 2., x * 3.],
        #         'vel': [0.01 * x, 0.01 * x, 0.01 * x],
        #         'N': [output_sizes.get(size)[1], output_sizes.get(size)[2], output_sizes.get(size)[3]],
        #         'distance': 1.1225,
        #         'mass': 1.0,
        #         'brownianFactor': 0.1,
        #         'brownianDIM': 3,
        #         'radius': 2.0
        #     })
    return data


def help_msg():
    print('usage: python generate_json.py [-s <size>][-o <outputfile>]')
    print('\tpossible value for size: s/small, m/medium, l/large')


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    file = ''
    size = 's'
    random = False
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hn:s:o:', ['help', 'size=', 'output='])
    except getopt.GetoptError as err:
        print(err)
        help_msg()
        sys.exit()
    for opt, arg in opts:
        if opt == '-h':
            help_msg()
            sys.exit()
        elif opt == '-n':
            shape_count = int(arg)
        elif opt in ('-s', '--size'):
            size = arg
        elif opt in ('-o', '--output'):
            file = arg

    if file == '':
        file = 'random_generated_output_' + size + '.json'

    data = write_json_benchmark(size)

    json_object = json.dumps(data, indent=4)

    with open(os.path.join(SAVE_DIR, file), 'w') as outfile:
        outfile.write(json_object)
