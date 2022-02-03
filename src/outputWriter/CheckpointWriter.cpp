#include <fstream>
#include "CheckpointWriter.h"
#include "nlohmann/json.hpp"
#include "physicsCalculator/LinkedCell.h"

using json = nlohmann::json;

void CheckpointWriter::writeCheckpoint(std::string &filename,
                                       ParticleContainer &container,
                                       PhysicsCalc &calc,
                                       int iteration) {
	auto lc = dynamic_cast<calculator::LinkedCell &>(calc);

	json j;
	j["iterations"] = iteration;
	j["particles"] = container;
	j["calc"] = lc;

	std::ofstream{filename} << j << std::endl;
}
