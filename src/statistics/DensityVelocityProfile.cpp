#include "DensityVelocityProfile.h"

#include "particleContainers/LinkedCellContainer.h"
#include "utils/ArrayUtils.h"

#include <fstream>
#include <utility>
#include <iostream>

namespace statistics {

DensityVelocityProfile::DensityVelocityProfile(std::string f, int noBins) : filename(std::move(f)),
                                                                            noBins(noBins) {
	std::ofstream file{folder_path + filename};
	file << "Iteration,Bin,Density,Velocity X,Velocity Y,Velocity Z\n";
}

void DensityVelocityProfile::writeStatistics(ParticleContainer &container, int iteration) {
	auto &particles = static_cast<LinkedCellContainer &>(container);

	double totalSize = particles.getLenDim()[0];
	double binSize = totalSize / noBins;
	std::vector<std::vector<Particle *>> bins;
	bins.reserve(noBins);
	bins.assign(noBins, {});

	for (auto &p : particles) {
		int binIndex = std::floor(p.getX()[0] / binSize);
		bins[binIndex].push_back(&p);
	}
	// open for appending
	std::ofstream file{folder_path + filename, std::ios::app};

	int binNumber = 0;
	for (auto &bin : bins) {
		if (bin.empty()) {
			file << iteration << "," << binNumber << ",-,-,-,-\n";
			binNumber++;
			continue;
		}
		auto noParts = static_cast<double>(bin.size());
		auto density = noParts / (binSize * particles.getLenDim()[1] * particles.getLenDim()[2]);

		auto totalVel = std::accumulate(bin.begin(), bin.end(), std::array<double, 3>{0., 0., 0.},
		                                [](auto acc, auto *p) { return p->getV() + acc; });
		auto avgVel = totalVel * std::array<double, 3>{1. / noParts, 1. / noParts, 1. / noParts};

		file << iteration << "," << binNumber << "," << density << "," << avgVel[0] << "," << avgVel[1] << ","
		     << avgVel[2] << "\n";
		binNumber++;
	}

}

} // end namespace statistics