#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"
#include "ParticleContainer.h"
#include "physicsCalculator/PhysicsCalc.h"
#include "physicsCalculator/Gravitation.h"
#include "physicsCalculator/LennardJones.h"
#include "utils/ParticleGenerator.h"
#include "utils/MaxwellBoltzmannDistribution.h"

#include <unistd.h>
#include <memory>
#include <string>
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

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
static IOWriter::iotype io_type{IOWriter::unknown};

/// Which Physics calculation method to use
static PhysicsCalc::calctype calc_type{PhysicsCalc::unknown};

/// Whether a particle generator getting parameters from the input file (for e.g. cuboids) should be used
static bool generate = false;
static bool randomGen = false;
static bool brownianMotion = false;
static double brownianMotionMean;
/**
 * @brief Parse command line arguments and set static values accordingly
 * also generates random input if no input file specified
 * @param argc argc from main
 * @param argv argv from main
 */
void get_arguments(int argc, char *argv[]) {
    const std::string help = "Usage: ./MolSim [-i <input_file>] [-t <input type>] [-e <end_time>] [-d <delta_t>] [-w <writer>] [-c <calc>] [-b <brownian_motion_velocity_mean>]\n"
                             "\tuse -i to specify an input file\n"
                             "\tuse -t to specify an input type: 'g'/'generate' to generate based on values from input_file, 'r'/'random' for random input (-i discarded for random)\n"
                             "\tuse -w to choose an output writer: v/vtk for VTKWriter (default) or x/xyz for XYZWriter\n"
                             "\tuse -c to choose a physics calculator: g/grav/gravitation for Gravitation, lj/lennardjones for LennardJonesPotential (default)\n"
                             // the situation with the generator is not very nice, but I want BrownianMotion optional for the generator but optional flag arguments are not supported
                             "\tuse -b to initialize particle movement with Brownian Motion (MaxwellBoltzmann) (not used for Generator generated particles)\n"
                             // "\tuse -r to enable random input generation (not used if -i is used)\n"
                             "\twhen leaving out -e/-d default values are used (1000/0.014)\n"
                             "\tcall with flag -h to display this message\n";
    int opt;
    if(argc == 1){
        std::cout << help;
        exit(0);
    }
    while ((opt = getopt(argc, argv, "hi:t:e:d:w:c:rb:")) != -1) {
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
                }else if (std::string("g") == optarg || std::string("grav") == optarg
                                                        || std::string("gravitation") == optarg) {
                    calc_type = PhysicsCalc::gravitation;
                }
                break;
            case 'b':
                brownianMotion = true;
                brownianMotionMean = std::stod(optarg);
                break;
            case '?':
                std::cerr << "Unknown option: " << static_cast<char>(optopt) << "\n";
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
            std::cout << "VTK-Writer" << std::endl;
            return std::make_unique<outputWriter::VTKWriter>();
        case IOWriter::xyz:
            std::cout << "XYZ-Writer" << std::endl;
            return std::make_unique<outputWriter::XYZWriter>();
        case IOWriter::unknown:
        default:
            std::cout << "VTK-Writer (Default)" << std::endl;
            return std::make_unique<outputWriter::VTKWriter>();
    }
}

static std::unique_ptr<PhysicsCalc> get_calculator() {
    std::cout << "\tCalculator: ";
    switch (calc_type) {
        case PhysicsCalc::gravitation:
            std::cout << "Gravitation" << std::endl;
            return std::make_unique<calculator::Gravitation>();
        case PhysicsCalc::lennardJones:
            std::cout << "Lennard-Jones-Potential" << std::endl;
            return std::make_unique<calculator::LennardJones>(1.,5.);
        case PhysicsCalc::unknown:
        default:
            std::cout << "Lennard-Jones-Potential (Default)" << std::endl;
            return std::make_unique<calculator::LennardJones>(1.,5.);
    }
}

void initializeParticles(ParticleContainer &particles) {
    // if generate flag is set and input for generator is specified, use ParticleGenerator
    if (generate && !filename.empty()) {
        ParticleGenerator::generateParticles(particles, filename);
        return;
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
    if(brownianMotion){
        initializeBrownianMotion(particles, brownianMotionMean);
    }
}

static void init_logger() {
    // Creates a logger, which writes everthing logged with spdlog::info() into build/logs/*
    auto logger = spdlog::basic_logger_mt("molsim_logger", "./logs/molsim.log");
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
}

int main(int argc, char *argv[]) {
    init_logger();

    // parse cmd line args and set static values accordingly
    get_arguments(argc, argv);

    std::cout << "Your configurations are:" << std::endl;
    std::cout << (randomGen?"\tRandom":"\tFilename: ") << filename << (generate?" (generator)":"") << std::endl;
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
    spdlog::info("Start calculating particles with\n\tIO type:\t\t{:<15}\n\tcalculator type:\t{:<15}\n\tend time:\t\t{:<15}\n\ttimestep:\t\t{:<15}", io->toString(), calc->toString(), end_time, delta_t);

    calc->calcF(particles);
    io->write(particles, "output", iteration);
    // for this loop, we assume: current x, current f and current v are known
    while (current_time < end_time) {
        spdlog::info("Iteration {}: ", iteration);
        // want to log like this: Moved x y z with velocity = {v} and force = {f}

        calc->calcX(particles);
        calc->calcF(particles);
        calc->calcV(particles);
        // want to log like this:  To   x y z with velocity = {v} and force = {f}

        iteration++;
        if (iteration % 10 == 0) {
            // uses abstract write method overwritten by specific IO method
            io->write(particles, "output", iteration);
        }

        current_time += delta_t;
    }

    std::cout << "All files have been written!" << std::endl;
    spdlog::info("All files have been written!");

    return 0;
}