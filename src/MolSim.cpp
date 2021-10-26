
#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"
#include "utils/ArrayUtils.h"
#include "ParticleContainer.h"
#include "Particle.h"
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"


#include <iostream>


/**** forward declaration of the calculation functions ****/

/**
 * calculate the force, position and velocity for all particles
 */
void calculateFXV(ParticleContainer&);

/**
 * plot the particles to a xyz-file or vtk-file
 * @param type 0 == xyz, 1 == vtk
 */
void plotParticles(int iteration, int type, const ParticleContainer&);

/// Returns the square of a number
template<typename T>
T sqr(T);

/// Computes the gravitational force between two particles for the first particle
///
/// @param p1 first particle
/// @param p2 second particle
void grav_force(Particle & p1, const Particle & p2);

static double start_time = 0;
static double end_time = 1000;
static double delta_t = 0.014;


log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("MolSim"));

int main(int argc, char *argv[]) {
    log4cxx::BasicConfigurator::configure();
    LOG4CXX_INFO(logger, "Hello from MolSim for PSE!");
    if (argc < 2 || argc > 5) {
        LOG4CXX_INFO(logger,"Erroneous programme call!");
        LOG4CXX_INFO(logger,"./MolSim filename [t_end] [delta_t] [configuration_file]");
    }

    if (argc == 3) {
        end_time = std::stod(argv[2]);
    } else if (argc == 4) {
        end_time = std::stod(argv[2]);
        delta_t = std::stod(argv[3]);
    } else if (argc == 5) {
        end_time = std::stod(argv[2]);
        delta_t = std::stod(argv[3]);
        log4cxx::PropertyConfigurator::configure(argv[4]);
    }
    ParticleContainer particles;

    FileReader::readFile(particles, argv[1]);

    double current_time = start_time;

    int iteration = 0;

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < end_time) {
        // calculate new x, f and v
        calculateFXV(particles);

        iteration++;
        if (iteration % 10 == 0) {
            plotParticles(iteration, 1, particles);
        }
        LOG4CXX_INFO(logger, "Iteration " << iteration << " finished");

        current_time += delta_t;
    }

    LOG4CXX_INFO(logger, "output written. Terminating...");
    return 0;
}

void calculateFXV(ParticleContainer& particles) {
    // calculates new force
    for (auto &p1 : particles) {
        for (auto &p2 : particles) {
            if (p1 != p2) {
                grav_force(p1, p2);
            }
        }
    }

    for (auto &p : particles) {
        // go through all three dimensions
        for (int i = 0; i < 3; ++i) {
            // calculates new position
            p.setX(i, p.getX().at(i) + delta_t * p.getV().at(i) + delta_t * delta_t * p.getF().at(i) * 0.5 / p.getM());

            // calculates new velocity
            p.setV(i, p.getV().at(i) + delta_t * 0.5 * (p.getF().at(i) + p.getOldF().at(i)) / p.getM());
        }
    }
}

void grav_force(Particle &p1, const Particle &p2) {
    double distance{};
    for (int i = 0; i < 3; ++i) {
        distance += sqr(p1.getX().at(i) - p2.getX().at(i));
        p1.setOldF(p1.getF());
        p1.setF(i, (p1.getM() * p2.getM() / (sqrt(distance) * distance)) * (p1.getX().at(i) - p2.getX().at(i)));
    }
}


void plotParticles(int iteration, int type, const ParticleContainer& particles) {
    std::string out_name("MD_vtk");
    // vtk
    if (type) {
        outputWriter::VTKWriter vtkWriter = outputWriter::VTKWriter{};
        vtkWriter.initializeOutput(static_cast<int>(particles.size()));
        for (const Particle& p : particles) {
            vtkWriter.plotParticle(p);
        }
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
