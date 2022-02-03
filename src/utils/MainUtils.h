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
#include "outputWriter/CheckpointWriter.h"
#include "inputReader/CheckpointReader.h"
#include "statistics/StatisticsLogger.h"
#include "statistics/DensityVelocityProfile.h"
#include "statistics/Thermodynamical.h"

#include <unistd.h>
#include <memory>
#include <string>
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <cfloat>

static std::vector<std::vector<double>> sigmaTable;

static std::vector<std::vector<double>> epsilonTable;

static std::vector<std::pair<int, std::pair<double, double>>> se_mapping;

struct MainUtils {

	/**
	 * @brief Parse command line arguments and set static values accordingly
	 * also generates random input if no input file specified
	 * @param argc argc from main
	 * @param argv argv from main
	 */
	static void get_arguments(int argc, char *argv[], Config &config);

	/**
	 * @brief Returns the (by cmd line arg) selected IO Method
	 * if (somehow) io_type is not set returns VTK
	 * @return a pointer to an Writer of the choosen IO Method
	 */
	static std::unique_ptr<IOWriter> get_io_type(Config &config);

	/**
	 * @brief Returns the (by cmd line arg) selected Calculation Method
	 * @return a pointer to an PhysicsCalc of the choosen Calculation Method
	 */
	static std::unique_ptr<PhysicsCalc> get_calculator(Config &config);

	static std::unique_ptr<ParticleContainer> get_container(Config &config);

	static Thermostat get_thermostat(Config &config);

	static std::unique_ptr<StatisticsLogger> get_statistics_logger(Config &config);

	/**
	 * Initializes the container according to the configs
	 * @param particles the particle container
	 * @param config the configurations
	 */
	static void initializeParticles(ParticleContainer &particles, Config &config);

	/**
	 * Logs the particles
	 * @param particles the particle container
	 */
	static void logParticle(ParticleContainer &particles);

	/**
	 * Parses the xml file according to the configuration
	 * @param config the configuration
	 */
	static void parseXML(Config &config);

	/**
	 * Prints the configurations onto the the console
	 * @param config the configurations
	 */
	static void printConfig(Config &config);

	/**
	 * Validates whether the input is valid (e.g., Particles too close to each other)
	 * @param config configuration
	 * @param particles the particle container
	 */
	static void validateInput(Config &config, ParticleContainer &particles);

	/**
	 * Initializes the logger
	 */
	static void initializeLogger();

private:

	/**
	 * Builds the sigma-epsilon Table
	 * @param mapping mapping of the sigma and epsilon values
	 */
	static void buildSETable(std::vector<std::pair<int, std::pair<double, double>>> &mapping);
};

