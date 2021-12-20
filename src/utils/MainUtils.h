#pragma once

#include "utils/Config.h"
#include "inputReader/FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"
#include "particleContainers/ParticleContainer.h"
#include "particleContainers/DirectSumParticleContainer.h"
#include "physicsCalculator/PhysicsCalc.h"
#include "physicsCalculator/Gravitation.h"
#include "physicsCalculator/LennardJones.h"
#include "physicsCalculator/LinkedCell.h"
#include "physicsCalculator/Thermostat.h"
#include "utils/ParticleGenerator.h"
#include "utils/MaxwellBoltzmannDistribution.h"
#include "inputReader/XMLReader.h"

#include <unistd.h>
#include <memory>
#include <string>
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// make config singleton?
static Config config;

static std::vector<std::vector<double>> sigmaTable;
static std::vector<std::vector<double>> epsilonTable;

struct MainUtils {

    /**
     * @brief Parse command line arguments and set static values accordingly
     * also generates random input if no input file specified
     * @param argc argc from main
     * @param argv argv from main
     */
    static void get_arguments(int argc, char *argv[]);

    /**
     * @brief Returns the (by cmd line arg) selected IO Method
     * if (somehow) io_type is not set returns VTK
     * @return a pointer to an Writer of the choosen IO Method
     */
    static std::unique_ptr<IOWriter> get_io_type();

    /**
     * @brief Returns the (by cmd line arg) selected Calculation Method
     * @return a pointer to an PhysicsCalc of the choosen Calculation Method
     */
    static std::unique_ptr<PhysicsCalc> get_calculator();

    static std::unique_ptr<ParticleContainer> get_container();

    static void initializeParticles(ParticleContainer &particles);

    static void logParticle(ParticleContainer &particles);

    static void parseXML();

    static void printConfig();

    static void initializeLogger();

private:

    static void buildSETable(std::vector<std::pair<int, std::pair<double, double>>> &mapping);


    /*
    /// Default start_time (end_time - start_time = total_runtime)
    static inline double start_time = 0;

    /// Default end_time (when to stop the simulation)
    static inline double end_time = 1000;

    /// Default delta t
    static inline double delta_t = 0.014;

    /// File name of the XML file specifying input parameters
    static inline std::string xml_file;

    /// File name used for the input file
    static inline std::vector<std::string> filename{};

    /// File name of the input to the ParticleGenerator
    static inline std::vector<std::string> generator_files{};

    /// base file name of the output files
    static inline std::string output_file = "output";

    /// Default dimension
    static inline int DIM = 3;

    /// Which IO method to use
    static inline IOWriter::iotype io_type{IOWriter::unknown};

    /// How often output files are generated (every nth file)
    static inline int writeFrequency = 10;

    /// Which Physics calculation method to use
    static inline  PhysicsCalc::calctype calc_type{PhysicsCalc::unknown};

    /// Whether to disable logging and IO operations
    static inline bool benchmarking = false;

    /// info for generator from XML file
    static inline std::vector<ParticleGenerator::ShapeInfo> generatorInfos{};

    /// Thermostat
    static inline Thermostat thermostat{};

    /// time steps to apply temperature
    static inline int nThermostat;

    /// initial temperature
    static inline double initialTemperature;

    /// temperature we want to reach
    static inline double targetTemperature;

    /// max temperature difference in one temperature calculation
    static inline double maxDeltaTemperature;


    static inline bool randomGen = false;
    static inline bool brownianMotion = false;
    static inline double brownianMotionMean;

    static inline double eps = 5.;
    static inline double sigma = 1.;

    static inline bool linkedCell;
    static inline std::array<int,3> linkedCellSize;
    static inline double rCut;
    static inline std::array<LinkedCellContainer::Border, 6> boundaryConditions; */
};

