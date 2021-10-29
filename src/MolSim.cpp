#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"
#include "utils/ArrayUtils.h"
#include "ParticleContainer.h"
#include "Particle.h"


#include <iostream>


/**** forward declaration of the calculation functions ****/

/**
 * calculate the force, position and velocity for all particles
 * @param ParticleContainer&: the container containing the particles
 */
void calculateXFV(ParticleContainer &particles);

/**
 * plot the particles to a xyz-file or vtk-file
 * @param type 0 == xyz, 1 == vtk
 */
void plotParticles(int iteration, int type, const ParticleContainer&);

/**
 * Returns the square of a number
 * @param T: the number
 */
template<typename T>
T sqr(T);

/** Computes the gravitational force between two particles for the first particle
 *
 * @param p1 first particle
 * @param p2 second particle
 */
void grav_force(Particle & p1, const Particle & p2);

// default end_time and delta_t
static double start_time = 0;
static double end_time = 1000;
static double delta_t = 0.014;

// default dimensions
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

    ParticleContainer particles;

    FileReader::readFile(particles, argv[1]);

    double current_time = start_time;

    int iteration = 0;

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < end_time) {
        // calculate new x, f and v
        calculateXFV(particles);

        iteration++;
        if (iteration % 10 == 0) {
            plotParticles(iteration, 1, particles);
        }

        std::cout<<"Iteration " << iteration << " finished"<<std::endl;

        current_time += delta_t;
    }

    return 0;
}

void calculateXFV(ParticleContainer& particles) {

    // calculate new positions
    for (auto &p : particles) { // loop over every particle
        // go through all three dimensions
        for (int i = 0; i < DIM; ++i) {
            // calculates new position
            double new_X = p.getX().at(i) + delta_t * (p.getV().at(i) + .5 / p.getM() * p.getF().at(i) * delta_t);
            p.setX(i, new_X);
            // set new force
            p.setF(i, p.getOldF().at(i));
        }
    }

    // calculates new force
    for (auto &p1 : particles) {
        for (auto &p2 : particles) {
            if (p1 != p2) {
                grav_force(p1, p2);
            }
        }
    }

    // calculate new velocities
    for (auto &p : particles) { // loop over every particle
        // go through all three dimensions
        for (int i = 0; i < DIM; ++i) {
            // calculates new velocity
            p.setV(i, p.getV().at(i) + delta_t * 0.5 * (p.getF().at(i) + p.getOldF().at(i)) / p.getM());
        }
    }

}

void grav_force(Particle &p1, const Particle &p2) {
    double sqrd_dist = 0;
    // calculate the squared distance
    for (int i = 0; i < DIM; ++i) {
        sqrd_dist += sqr(p2.getX().at(i) - p1.getX().at(i));
    }
    // left side of the term
    double var = p1.getM() * p2.getM() / (sqrt(sqrd_dist) * sqrd_dist);
    // multiplying with (p2 - p1) and setting the force
    for (int i = 0; i < DIM; ++i) {
        p1.setF(i, var * (p2.getX().at(i) - p1.getX().at(i)));
    }
}


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

template<typename T>
T sqr(T x) {
    return x * x;
}
