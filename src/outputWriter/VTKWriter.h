/*
 * VTKWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include "Particle.h"
#include "outputWriter/vtk-unstructured.h"
#include "IOWriter.h"

#include <list>

namespace outputWriter {

/**
 * This class implements the functionality to generate vtk output from
 * particles.
 */
    class VTKWriter : public IOWriter {

    public:
        VTKWriter();

        virtual ~VTKWriter();

        // TODO adapt to LinkedCellContainer
        /**
         * override of function IOWRriter::write for VTK
         * @param container the ParticleContainer containg all particles that should be output
         * @param filename custom filename
         * @param iteration current iteration, used for unique filenames
         */
        void write(const DirectSumParticleContainer &container, const std::string &filename, int iteration) const override;

    private:

        /**
       * set up internal data structures and prepare to plot a particle.
       */
        void initializeOutput(int numParticles) const;

        /**
         * plot type, mass, position, velocity and force of a particle.
         *
         * @note: initializeOutput() must have been called before.
         */
        void plotParticle(const Particle &p) const;

        /**
         * writes the final output file.
         *
         * @param filename the base name of the file to be written.
         * @param iteration the number of the current iteration,
         *        which is used to generate an unique filename
         */
        void writeFile(const std::string &filename, int iteration) const;
        std::string toString() override;
        VTKFile_t *vtkFile;
    };


} // namespace outputWriter
