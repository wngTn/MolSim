#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"
#include "ParticleContainer.h"
#include "Particle.h"
#include <unistd.h>
#include <memory>
#include <string>
#include <iostream>


/**** forward declaration of the calculation functions ****/

/**
 * Plot the particles to a xyz-file or vtk-file
 * @param iteration the iteration
 * @param type 0 == xyz, 1 == vtk
 * @param particles the container with the particles
 */
void plotParticles(int iteration, int type, const ParticleContainer& particles);


// default end_time and delta_t
static double start_time = 0;
static double end_time = 1000;
static double delta_t = 0.014;
static std::string filename;
// default dimension
static int DIM = 3;

// which IO methode to use
static IOWriter::iotype io_type;

/**
 * parse command line arguments and set static values accordingly
 * also generates random input if no input file specified
 * @param argc argc from main
 * @param argv argv from main
 */
void get_arguments(int argc, char *argv[]){
    const std::string help = "Usage: ./MolSim [-i <input_file>] [-e <end_time>] [-d <delta_t>] [-v | -x]\n"
                             "\tuse -v to enable vtk output and -x to enable xyz output (default vtk)\n"
                             "\twhen leaving out -e/-d default values are used (1000/0.014)\n"
                             "\twhen leaving out -i a random input file is generated automatically \n"
                             "\tcall with flag -h to display this message\n";

    int opt;
    while((opt = getopt(argc, argv, "hi:e:d:vx")) != -1){
        switch(opt){
            case 'h':
                std::cout << help;
                return;
            case 'i':
                filename = optarg;
                break;
            case 'e':
                end_time = std::stod(optarg);
                break;
            case 'd':
                delta_t = std::stod(optarg);
                break;
            case 'v':
                io_type = IOWriter::vtk;
                break;
            case 'x':
                io_type = IOWriter::xyz;
                break;
            case '?':
                std::cerr << "Unknown option: " << optopt << "\n";
                std::cerr << help;
                break;
            default:
                break;
        }

    }

    // if no input file has been specified, generate random input using python script
    if(filename.empty()){
        std::cout << "Generating random input (this needs python to be installed)\n";
        // maybe change particle amount
        std::system("python ../generate_input.py -n 50 -o input.txt");
        filename = "input.txt";
    }
}

/**
 * returns the (by cmd line arg) selected IO Methode
 * if (somehow) io_type is not set returns VTK
 * @return a pointer to an Writer of the choosen IO Method
 */
static std::unique_ptr<IOWriter> get_io_type(){
    switch(io_type){
        case IOWriter::vtk:
            return std::make_unique<outputWriter::VTKWriter>();
        case IOWriter::xyz:
            return std::make_unique<outputWriter::XYZWriter>();
        default:
            return std::make_unique<outputWriter::VTKWriter>();
    }
}

int main(int argc, char *argv[]) {
    std::cout<<"Hello from MolSim for PSE!"<<std::endl;

    // parse cmd line args and set static values accordingly
    get_arguments(argc, argv);

    // create particle container
    ParticleContainer particles = ParticleContainer(DIM, delta_t);
    // read input file
    FileReader::readFile(particles, filename);

    // set IO Method according to io_type flag obtained from cmd line args
    // which type exactly it is not important here

    //std::unique_ptr<IOWriter> io;
    auto io = get_io_type();
    /* switch(io_type){
        case IOWriter::vtk:
            io = std::make_unique<outputWriter::VTKWriter>();
            break;
        case IOWriter::xyz:
            io = std::make_unique<outputWriter::XYZWriter>();
            break;
        default:
            io = std::make_unique<outputWriter::VTKWriter>();
            break;
    } */

    double current_time = start_time;

    int iteration = 0;

    particles.calculateF();
    // for this loop, we assume: current x, current f and current v are known
    while (current_time < end_time) {

        particles.calculateX();

        particles.calculateF();

        particles.calculateV();

        iteration++;
        if (iteration % 10 == 0) {
            // uses abstract write method overwritten by specific IO method
            io->write(particles, "output" , iteration);
        }

        std::cout<<"Iteration " << iteration << " finished"<<std::endl;

        current_time += delta_t;
    }

    return 0;
}
