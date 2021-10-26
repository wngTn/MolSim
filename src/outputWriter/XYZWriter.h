/*
 * XYZWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include "Particle.h"
#include "ParticleContainer.h"

#include <fstream>
#include <vector>

namespace outputWriter {

class XYZWriter {

public:
  XYZWriter();

  virtual ~XYZWriter();

  static void plotParticles(const ParticleContainer & particles, const std::string &filename,
                     int iteration);
};

} // namespace outputWriter
