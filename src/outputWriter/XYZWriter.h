/*
 * XYZWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include "Particle.h"
#include "particleContainers/DirectSumParticleContainer.h"
#include "IOWriter.h"

#include <fstream>
#include <vector>

namespace outputWriter {

    class XYZWriter : public IOWriter {

    public:
        XYZWriter();

        virtual ~XYZWriter();

        /**
         * override of function IOWRriter::write for VTK
         * @param container the ParticleContainer containg all particles that should be output
         * @param filename custom filename
         * @param iteration current iteration, used for unique filenames
         */
        void write(const ParticleContainer &container, const std::string &filename, int iteration) const override;
        std::string toString() override;

    private:
        /* void plotParticles(const ParticleContainer & particles, const std::string &filename,
                                   int iteration) const;
     */
    };

} // namespace outputWriter
