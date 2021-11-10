#pragma once

#include "ParticleContainer.h"
#include "Particle.h"


class IOWriter {

public:
    enum iotype {
        vtk,
        xyz,
        unknown
    };

    /**
     * writes an output file of some form
     * @param container the ParticleContainer containing all particles that should be output
     * @param filename custom filename
     * @param iteration current iteration, used for unique filenames
     */
    virtual void write(const ParticleContainer &container, const std::string &filename, int iteration) const = 0;
    virtual std::string toString() = 0;
};
