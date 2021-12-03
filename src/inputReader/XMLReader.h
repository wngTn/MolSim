#pragma once
#include "../outputWriter/IOWriter.h"
#include "../physicsCalculator/PhysicsCalc.h"
#include "../utils/ParticleGenerator.h"
#include "../particleContainers/LinkedCellContainer.h"
#include "simulation.h"

#include <vector>
#include <string>


class XMLReader {

public:

    static void initializeBorderType(int index, border_single_t& type, std::array<LinkedCellContainer::Border, 6>& boundaryConds);

    struct XMLInfo {
        // general parameters of the simulation
        double epsilon;
        double sigma;
        double t_end;
        double delta_t;
        int writeFrequency;
        double brownianMotionMean;
        // whether randomly generated particles should be used, other inputs discarded if yes
        bool random;
        // output and calculation types
        IOWriter::iotype outputWriterType;
        PhysicsCalc::calctype calculatorType;
        // all inputFiles (format of week 1)
        std::vector<std::string> inputFiles;
        // the base name of the output files
        std::string outputfile;
        // ParticleGenerator input that has directly been specified in the XML file
        std::vector<ParticleGenerator::ShapeInfo> generatorInfos;
        // input files (JSON) for the ParticleGenerator, for specifications see info about ParticleGenerator
        std::vector<std::string> generatorInputFiles;
        //stuff used for linked cell
        bool linkedcell;
        std::array<int,3> linkedCellSize;
        double rCut;
        std::array<LinkedCellContainer::Border, 6> boundaryConditions;
    };

    static XMLInfo readFile(const std::string& s);
private:
    static void insertGeneratorInfo(std::vector<ParticleGenerator::ShapeInfo>& genInfos, generator_info_t &info);
};
