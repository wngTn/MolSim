#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"
#include "ParticleContainer.h"
#include "Particle.h"


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

    if (argc < 2 || argc > 4) {
        std::cout<<"Erroneous programme call!"<<std::endl;
        std::cout<<"./MolSim filename [t_end] [delta_t]"<<std::endl;
    }

    // if no t_end or delta_t is provided, we use the defaults
    if (argc == 3) {
        end_time = std::stod(argv[2]);
    } else if (argc == 4) {
        end_time = std::stod(argv[2]);
        delta_t = std::stod(argv[3]);
    }

    ParticleContainer particles = ParticleContainer(DIM, delta_t);

    FileReader::readFile(particles, argv[1]);

    outputWriter::VTKWriter vtk{};
    IOWriter& io = vtk;

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

/* obsolete, can be removed
void plotParticles(int iteration, int type, const ParticleContainer& particles) {
    // The name of the files
    std::string out_name("MD_vtk");
    // vtk
    if (type) {
        // initializing the VTKWriter
        outputWriter::VTKWriter vtkWriter = outputWriter::VTKWriter{};
        // initializing the Output
        vtkWriter.initializeOutput(static_cast<int>(particles.size()));
        // plotting for every particle
        for (const Particle& p : particles) {
            vtkWriter.plotParticle(p);
        }
        // writes the file
        vtkWriter.writeFile(out_name, iteration);
    }
    // xyz
    else {
        outputWriter::XYZWriter::plotParticles(particles, out_name, iteration);
    }
}
*/