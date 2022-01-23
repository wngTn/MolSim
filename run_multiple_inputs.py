import os

# run this script in the MolSimRepo/ directory!
# this is probably simple enough for a bash script but I've never written one of those so python it is

folder_path = '../input/files/assignment_5/task4_experiments/'

# here you can add new files, just add the file name and the identifier,
# output file name needs to end with the identifier and statistics should contain it as well
files: [(str, str)] = [(folder_path + 'task4_assignment.xml', 'assignment'),
                       (folder_path + 'task4_walls_equal_fluid.xml', 'walleqfluid'),
                       (folder_path + 'task4_higher_gravity.xml', 'highgrav'),
                       (folder_path + 'task4_fixed_spheres.xml'), 'fixedspheres']


def execute_file(path: str, identifier: str):
    os.system(f'cd build/ && ./MolSim -x {path}')
    print(f'finished running {identifier}')

    os.system(f'cd build/ && zip -9 {identifier}.zip *_{identifier}_*.vtu')
    print(f'finished zipping {identifier}')

    os.system(f'cd build/ && rm *_{identifier}_*.vtu')


def execute_all():
    os.system('cd build/ && cmake .. && make')
    for [path, match] in files:
        execute_file(path, match)


execute_all()
