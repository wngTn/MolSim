/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include "Particle.h"
#include "DirectSumParticleContainer.h"

#include <vector>

class FileReader {

public:
    FileReader();

    virtual ~FileReader();

    static void readFile(DirectSumParticleContainer &particles, const std::string &filename);
};
