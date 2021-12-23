#pragma once

#include <outputWriter/IOWriter.h>
#include <physicsCalculator/PhysicsCalc.h>
#include <physicsCalculator/Thermostat.h>
#include <particleContainers/LinkedCellContainer.h>
#include "ParticleGenerator.h"

struct Config {
public:
/// iteration to start with, used for checkpointing
    int start_iteration;
/// Default start_time (end_time - start_time = total_runtime)
    double start_time = 0;

/// Default end_time (when to stop the simulation)
    double end_time = 1000;

/// Default delta t
    double delta_t = 0.014;

/// File name of the XML file specifying input parameters
    std::string xml_file;

/// File name used for the input file
    std::vector <std::string> filename{};

/// File name of the input to the ParticleGenerator
    std::vector <std::string> generator_files{};

/// base file name of the output files
    std::string output_file = "output";

/// whether checkpointing is used
    bool checkpointing;

/// input file from a previous checkpoint
    std::string checkpointInput;

/// filename for checkpointing output
    std::string checkpointOutput;

/// Default dimension
    int DIM = 3;

/// Which IO method to use
    IOWriter::iotype io_type{IOWriter::unknown};

/// How often output files are generated (every nth file)
    int writeFrequency = 10;

/// Which Physics calculation method to use
    PhysicsCalc::calctype calc_type{PhysicsCalc::unknown};

/// Whether to disable logging and IO operations
    bool benchmarking = false;

/// info for generator from XML file
    std::vector <ParticleGenerator::ShapeInfo> generatorInfos{};

/// time steps to apply temperature
    int nThermostat;

/// whether a thermostat is used
    bool useThermostat = false;

/// initial temperature
    double initialTemperature;

/// temperature we want to reach
    double targetTemperature;

/// max temperature difference in one temperature calculation
    double maxDeltaTemperature;


    bool randomGen = false;
    bool brownianMotion = false;
    double brownianMotionMean;

    double eps = 5.;
    double sigma = 1.;
    double grav;

    bool linkedCell;
    std::array<int, 3> linkedCellSize;
    double rCut;
    std::array<LinkedCellContainer::Border, 6> boundaryConditions;

};