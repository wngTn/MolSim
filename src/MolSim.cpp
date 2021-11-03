#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"
#include "ParticleContainer.h"
#include "Particle.h"
#include <unistd.h>
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


// default dimension
static int DIM = 3;


int main(int argc, char *argv[]) {
    std::cout<<"Hello from MolSim for PSE!"<<std::endl;
    const std::string help = "Usage: ./MolSim [-i <input_file>] [-e <end_time>] [-d <delta_t>]\n"
                             "\twhen leaving out -e/-d default values are used (1000/0.014)\n"
                             "\twhen leaving out -i a random input file is generated automatically \n"
                             "\tcall with flag -h to display this message\n";

    int opt;
    std::string filename;
    while((opt = getopt(argc, argv, "hi:e:d:")) != -1){
        switch(opt){
            case 'h':
                std::cout << help;
                return 0;
            case 'i':
                filename = optarg;
                break;
            case 'e':
                end_time = std::stod(optarg);
                break;
            case 'd':
                delta_t = std::stod(optarg);
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

    ParticleContainer particles = ParticleContainer(DIM, delta_t);

    FileReader::readFile(particles, filename);

    const IOWriter& io = outputWriter::VTKWriter{};

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
            io.write(particles, "output" , iteration);
            // plotParticles(iteration, 1, particles);
        }

        std::cout<<"Iteration " << iteration << " finished"<<std::endl;

        current_time += delta_t;
    }

    return 0;
}
