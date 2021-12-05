/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include "Particle.h"
#include "particleContainers/ParticleContainer.h"

#include <vector>

class FileReader {

public:
    FileReader();

    virtual ~FileReader();

    static void readFile(ParticleContainer &particles, const std::string &filename);
};
