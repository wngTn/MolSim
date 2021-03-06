#pragma once

#include "particleContainers/ParticleContainer.h"

class ParticleGenerator {
public:
	static std::vector<std::pair<int, std::pair<double, double>>> generateParticles(ParticleContainer &particles,
	                                                                                const std::string &file);

	static std::vector<std::pair<int, std::pair<double, double>>> generateParticles(ParticleContainer &particles,
	                                                                                const std::string &file,
	                                                                                int startSEIndex);

	enum geometric_type {
		cuboid,
		sphere
	};

	enum behaviour_type {
		normal,
		membrane,
		immovable
	};

	struct ShapeInfo {
		geometric_type type;

		behaviour_type behaviour;
		// for sphere AND cuboid
		// C: lower left front side corner, S: center
		std::array<double, 3> pos;
		// initial velocity of the particles
		std::array<double, 3> vel;
		// mass of particles
		double mass;

		double sigma;

		double epsilon;
		// distance between particles
		double distance;
		// mean velocity of brownian motion
		double brownianFactor;
		int DIM;
		// only used for sphere
		int radius;
		// only used for cuboid
		std::array<int, 3> N;
		// base Force applied to each particle for each time step. only applied until reset
		std::array<double, 3> baseForce = {0, 0, 0};
		// this is kind ugly but easy
		// stores info for special particles: position (in grid), special force, special start V, special mass, immovable, membrane, type
		std::vector<std::tuple<std::array<int, 3>,
		                       std::array<double, 3>,
		                       std::array<double, 3>,
		                       double,
		                       bool,
		                       bool,
		                       int>> specialParticles;
	};

	static std::vector<std::pair<int, std::pair<double, double>>> generateParticles(ParticleContainer &particles,
	                                                                                const std::vector<ShapeInfo> &infovec);

	static std::vector<std::pair<int, std::pair<double, double>>> generateParticles(ParticleContainer &particles,
	                                                                                const std::vector<ShapeInfo> &infovec,
	                                                                                int startSEIndex);

private:

	/**
	 * parses a JSON file and returns the content (if well formed) as a vector of ShapeInfo object
	 * @param file the path to the input file
	 * @return a vector containing ShapeInfo Objects for all shapes that should be generated
	 */
	static std::vector<ShapeInfo> readJSON(const std::string &file);

	/**
	 * generates a cuboid specified by the passed parameters
	 * @param particles the ParticleContainer the created cube is stored in
	 * @param info the ShapeInfo struct containing the parameters
	 */
	static void generateCuboid(ParticleContainer &particles, const ShapeInfo &info, int startSEIndex);


	// this creates a cube and cuts the sphere out of it. not very cool sphere but min distance is kept
	/**
	 * generates a sphere specified by the passed parameters
	 * @param particles the ParticleContainer the created sphere is stored in
	 * @param info the ShapeInfo struct containing the parameters
	 */
	static void generateSphere(ParticleContainer &particles, const ShapeInfo &info, int startSEIndex);

	// currently not used, testing other methods for sphere generation
	// this produces veeeery smooth & nice spheres, but the minimum distance is not kept, so not suited for LJP
	// also not optimized at all bc still WIP
	static void generateSphere2(ParticleContainer &particles, const ShapeInfo &info);
};

