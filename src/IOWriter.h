//
// Created by sev on 30.10.21.
//

#ifndef PSEMOLDYN_GROUPC_IOWRITER_H
#define PSEMOLDYN_GROUPC_IOWRITER_H
#include "ParticleContainer.h"
#include "Particle.h"


class IOWriter {

public:
/**
 * writes an output file of some form
 * @param container the ParticleContainer containing all particles that should be output
 * @param filename custom filename
 * @param iteration current iteration, used for unique filenames
 */
virtual void write(const ParticleContainer &container, const std::string &filename,  int iteration) = 0;
};


#endif //PSEMOLDYN_GROUPC_IOWRITER_H
