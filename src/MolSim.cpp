#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"
#include "ParticleContainer.h"
#include "physicsCalculator/PhysicsCalc.h"
#include "physicsCalculator/StoermerVerlet.h"
#include "physicsCalculator/LennardJones.h"
#include "utils/ParticleGenerator.h"

#include <unistd.h>
#include <memory>
#include <string>
#include <iostream>

/// Default start_time (end_time - start_time = total_runtime)
static double start_time = 0;
/// Default end_time (when to stop the simulation)
static double end_time = 1000;
/// Default delta t
static double delta_t = 0.014;

/// File name used for the input/output file(s)
static std::string filename;

/// Default dimension
static int DIM = 3;

/// Which IO method to use
static IOWriter::iotype io_type;

/// Which Physics calculation method to use
static PhysicsCalc::calctype calc_type;

/// Whether a particle generator getting parameters from the input file (for e.g. cuboids) should be used
static bool generate = false;
static bool randomGen = false;
static bool brownianMotion = false;
/**
 * @brief Parse command line arguments and set static values accordingly
 * also generates random input if no input file specified
 * @param argc argc from main
 * @param argv argv from main
 */
void get_arguments(int argc, char *argv[]) {
    const std::string help = "Usage: ./MolSim [-i <input_file>] [-t <input type>] [-e <end_time>] [-d <delta_t>] [-w <writer>] [-c <calc>] [-r] \n"
                             "\tuse -i to specify an input file\n"
                             "\tuse -t to specify an input type: 'g'/'generate' to generate based on values from input_file, 'r'/'random' for random input (-i discarded for random)\n"
                             "\tuse -w to choose an output writer: v/vtk for VTKWriter (default) or x/xyz for XYZWriter\n"
                             "\tuse -c to choose a physics calculator: sv/stoermerverlet for StoermerVerlet (default), lj/lennardjones for LennardJonesPotential\n"
                             "\tuse -b to initialize particle movement with Brownian Motion (MaxwellBoltzmann) (used by default if LennardJonesPotential is used)\n"
                            // "\tuse -r to enable random input generation (not used if -i is used)\n"
                             "\twhen leaving out -e/-d default values are used (1000/0.014)\n"
                             "\tcall with flag -h to display this message\n";

    int opt;

    if(argc == 1){
        std::cout << help;
        exit(0);
    }
    while ((opt = getopt(argc, argv, "hi:t:e:d:w:c:rb")) != -1) {
        switch (opt) {
            case 'h':
                std::cout << help;
                exit(0);
            case 'i':
                filename = optarg;
                break;
            case 't':
                if (std::string("g") == optarg || std::string("generate") == optarg) {
                    generate = true;
                }else if (std::string("r") == optarg || std::string("random") == optarg) {
                    randomGen = true;
                }
                break;
            case 'e':
                end_time = std::stod(optarg);
                break;
            case 'd':
                delta_t = std::stod(optarg);
                break;
            case 'w':
                if (std::string("vtk") == optarg || std::string("v") == optarg) {
                    io_type = IOWriter::vtk;
                }else if (std::string("xyz") == optarg || std::string("x") == optarg) {
                    io_type = IOWriter::xyz;
                }
                break;
            case 'c':
                if (std::string("lj") == optarg || std::string("lennardjones") == optarg) {
                    calc_type = PhysicsCalc::lennardJones;
                    brownianMotion = true; //always use brownianMotion of LJP is used
                }else if (std::string("sv") == optarg || std::string("stoermerverlet") == optarg) {
                    calc_type = PhysicsCalc::stoermerVerlet;
                }
                break;
    //        case 'r':
    //            random = true;
    //            break;
            case 'b':
                brownianMotion = true;
                break;
            case '?':
                std::cerr << "Unknown option: " << optopt << "\n";
                std::cerr << help;
                break;
            default:
                break;
        }
    }

}

/**
 * @brief Returns the (by cmd line arg) selected IO Methode
 * if (somehow) io_type is not set returns VTK
 * @return a pointer to an Writer of the choosen IO Method
 */
static std::unique_ptr<IOWriter> get_io_type() {
    std::cout << "\tWriter: ";
    switch (io_type) {
        case IOWriter::vtk:
            std::cout << "VTK-Writer (Default)" << std::endl;
            return std::make_unique<outputWriter::VTKWriter>();
        case IOWriter::xyz:
            std::cout << "XYZ-Writer" << std::endl;
            return std::make_unique<outputWriter::XYZWriter>();
        default:
            return std::make_unique<outputWriter::VTKWriter>();
    }
}

static std::unique_ptr<PhysicsCalc> get_calculator() {
    std::cout << "\tCalculator: ";
    switch (calc_type) {
        case PhysicsCalc::stoermerVerlet:
            std::cout << "Stoermer-Verlet (Default)" << std::endl;
            return std::make_unique<calculator::StoermerVerlet>();
        case PhysicsCalc::lennardJones:
            std::cout << "Lennard-Jones-Potential" << std::endl;
            return std::make_unique<calculator::LennardJones>();
        default:
            return std::make_unique<calculator::StoermerVerlet>();
    }
}

void initializeParticles(ParticleContainer &particles) {
    // if generate flag is set and input for generator is specified, use ParticleGenerator
    if (generate && !filename.empty()) {
        ParticleGenerator::generateParticles(particles, filename, brownianMotion);

    // if no input file has been specified, generate random input using python script
    }else if (randomGen && filename.empty()) {
        std::cout << "No input file specified, generating random input... (this needs python to be installed)\n";
        // maybe change particle amount
        std::system("python ../generate_input.py -n 24 -o input.txt");
        filename = "input.txt";
    }else if (filename.empty()){
        std::cout << "Please specify an input file" << std::endl;
    }else{
        // read input file
        FileReader::readFile(particles, filename);
    }
}

int main(int argc, char *argv[]) {

    // parse cmd line args and set static values accordingly
    get_arguments(argc, argv);

    std::cout << "Your configurations are:" << std::endl;
    std::cout << "\tFilename: " << filename << std::endl;
    std::cout << "\tEnd_time: " << end_time << (end_time == 1000 ? "(Default)" : "") << std::endl;
    std::cout << "\tDelta_t: " << delta_t << (delta_t == 0.014 ? "(Default)" : "") << std::endl;


    // create particle container
    ParticleContainer particles = ParticleContainer();

    initializeParticles(particles);

    auto io = get_io_type();

    auto calc = get_calculator();

    calc->setDim(DIM);
    calc->setDeltaT(delta_t);

    double current_time = start_time;

    int iteration = 0;

    std::cout << "Currently processing your request..." << std::endl;

    calc->calcF(particles);
    // for this loop, we assume: current x, current f and current v are known
    while (current_time < end_time) {
        calc->calcX(particles);
        calc->calcF(particles);
        calc->calcV(particles);

        iteration++;
        if (iteration % 10 == 0) {
            // uses abstract write method overwritten by specific IO method
            io->write(particles, "output", iteration);
        }

        // std::cout<<"Iteration " << iteration << " finished"<<std::endl;

        current_time += delta_t;
    }

    std::cout << "All files have been written!" << std::endl;

    return 0;
}
