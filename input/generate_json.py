import json
import os
import getopt
import sys

# points to input directory
ROOT_DIR = os.path.dirname(os.path.abspath(__file__))

# points to where we want to save the files
SAVE_DIR = os.path.join(ROOT_DIR, 'files')

output_sizes = {
    's': [3, 2, 10, 2],  # -> 120 particles
    'small': [3, 2, 10, 2],
    'm': [3, 3, 12, 4],  # -> 432 particles
    'medium': [3, 3, 12, 4],
    'l': [4, 3, 12, 4],  # -> 576 particles
    'large': [4, 3, 12, 4]
}


def write_json(size):
    if size not in ('s', 'small', 'm', 'medium', 'l', 'large'):
        print("Didn't specify a vlid size, uses default (small.")
        print("Valid sizes are: s, small, m, medium, l and large.")

    data = {'shapes': []}
    for x in range(output_sizes.get(size)[0]):
        # if x % 2 == 0:
        data['shapes'].append({
            'type': 'cuboid',
            'pos': [x * 10, x * 20, x * 0.5],
            'vel': [(-1) ** x * 0.1 * x, 0.8 * x, (-1) ** x * 0.4 * x],
            'N': [output_sizes.get(size)[1], output_sizes.get(size)[2], output_sizes.get(size)[3]],
            'distance': 3.1225,
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
    #         'radius': 1.0
    #     })
    return data


def help_msg():
    print('usage: python generate_json.py [-s <size>][-o <outfile>]')
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
        if size in ('s', 'small'):
            file = 'automatic_generated_input_s.json'
        elif size in ('m', 'medium'):
            file = 'automatic_generated_input_m.json'
        elif size in ('l', 'large'):
            file = 'automatic_generated_input_l.json'

    data = write_json(size)

    json_object = json.dumps(data, indent=4)

    with open(os.path.join(SAVE_DIR, file), 'w') as outfile:
        outfile.write(json_object)
