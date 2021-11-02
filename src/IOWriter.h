//
// Created by sev on 30.10.21.
//

#ifndef PSEMOLDYN_GROUPC_IOWRITER_H
#define PSEMOLDYN_GROUPC_IOWRITER_H
#include "ParticleContainer.h"
#include "Particle.h"


class IOWriter {
    virtual void write(ParticleContainer) = 0;
};


#endif //PSEMOLDYN_GROUPC_IOWRITER_H
