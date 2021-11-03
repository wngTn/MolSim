
#ifndef PSEMOLDYN_GROUPC_IOWRITER_H
#define PSEMOLDYN_GROUPC_IOWRITER_H
#include "ParticleContainer.h"
#include "Particle.h"


class IOWriter {

public:
    enum iotype{vtk, xyz};

/**
 * writes an output file of some form
 * @param container the ParticleContainer containing all particles that should be output
 * @param filename custom filename
 * @param iteration current iteration, used for unique filenames
 */
virtual void write(const ParticleContainer &container, const std::string &filename,  int iteration) const = 0;
};


#endif //PSEMOLDYN_GROUPC_IOWRITER_H
