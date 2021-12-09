/*
 * VTKWriter.cpp
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#include "VTKWriter.h"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <spdlog/spdlog.h>

namespace outputWriter {

    VTKWriter::VTKWriter() : vtkFile{new VTKFile_t("UnstructuredGrid")} {
    }

    VTKWriter::~VTKWriter() {
        delete vtkFile;
    }

    void VTKWriter::initializeOutput(int numParticles) const {
        // per point, we add type, position, velocity and force
        PointData pointData;
        DataArray_t mass(type::Float32, "mass", 1);
        DataArray_t velocity(type::Float32, "velocity", 3);
        DataArray_t forces(type::Float32, "force", 3);
        DataArray_t type(type::Int32, "type", 1);
        pointData.DataArray().push_back(mass);
        pointData.DataArray().push_back(velocity);
        pointData.DataArray().push_back(forces);
        pointData.DataArray().push_back(type);

        CellData cellData; // we don't have cell data => leave it empty

        // 3 coordinates
        Points points;
        DataArray_t pointCoordinates(type::Float32, "points", 3);
        points.DataArray().push_back(pointCoordinates);

        Cells cells; // we don't have cells, => leave it empty
        // for some reasons, we have to add a dummy entry for paraview
        DataArray_t cells_data(type::Float32, "types", 0);
        cells.DataArray().push_back(cells_data);

        PieceUnstructuredGrid_t piece(pointData, cellData, points, cells,
                                      numParticles, 0);
        UnstructuredGrid_t unstructuredGrid(piece);
        vtkFile->UnstructuredGrid(unstructuredGrid);
    }

    void VTKWriter::writeFile(const std::string &filename, int iteration) const {
        std::stringstream strstr;
        strstr << filename << "_" << std::setfill('0') << std::setw(4) << iteration << ".vtu";

        std::ofstream file(strstr.str().c_str());
        VTKFile(file, *vtkFile);
    }

    void VTKWriter::plotParticle(const Particle &p) const {
        if (vtkFile->UnstructuredGrid().present()) {
            spdlog::info("UnstructuredGrid is present");
        } else {
            spdlog::critical("ERROR: No UnstructuredGrid present");
        }

        PointData::DataArray_sequence &pointDataSequence =
                vtkFile->UnstructuredGrid()->Piece().PointData().DataArray();
        PointData::DataArray_iterator dataIterator = pointDataSequence.begin();

        dataIterator->push_back(p.getM());
        spdlog::info("Appended mass data in: {}", dataIterator->Name());

        dataIterator++;
        dataIterator->push_back(p.getV()[0]);
        dataIterator->push_back(p.getV()[1]);
        dataIterator->push_back(p.getV()[2]);
        spdlog::info("Appended velocity data in: {}",dataIterator->Name());

        dataIterator++;
        dataIterator->push_back(p.getOldF()[0]);
        dataIterator->push_back(p.getOldF()[1]);
        dataIterator->push_back(p.getOldF()[2]);
        spdlog::info("Appended force data in: {}", dataIterator->Name());

        dataIterator++;
        dataIterator->push_back(p.getType());

        Points::DataArray_sequence &pointsSequence =
                vtkFile->UnstructuredGrid()->Piece().Points().DataArray();
        Points::DataArray_iterator pointsIterator = pointsSequence.begin();
        pointsIterator->push_back(p.getX()[0]);
        pointsIterator->push_back(p.getX()[1]);
        pointsIterator->push_back(p.getX()[2]);
    }

    void VTKWriter::write(const ParticleContainer &container, const std::string &filename, int iteration) const {
        // currently just using the already implemented methods, could be combined
        this->initializeOutput(
                static_cast<int>(container.size())); //assuming there are not more than int.MAX_VAL particles
        for (const Particle &p: container) {
            this->plotParticle(p);
        }
        this->writeFile(filename, iteration);
    }

std::string VTKWriter::toString() {
    return "vtk";
}

} // namespace outputWriter
