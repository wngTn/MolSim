#include "Thermodynamical.h"
#include "utils/ArrayUtils.h"

#include <utility>
#include <fstream>
#include <particleContainers/LinkedCellContainer.h>
#include <iostream>

namespace statistics {

Thermodynamical::Thermodynamical(std::string file, double delta_r, double maxDistance) : filename{std::move(file)},
                                                                                         delta_r{delta_r},
                                                                                         max_distance{maxDistance} {
	std::ofstream f_rdf{folder_path + "RDF_" + filename};
	std::ofstream f_diff{folder_path + "diffusion_" + filename};
	f_rdf << "Iteration,Interval,Density\n";
	f_diff << "Iteration,Diffusion\n";
}

double Thermodynamical::calculateDiffusion(ParticleContainer &container) {
	auto &particles = static_cast<LinkedCellContainer &>(container);
	auto size = particles.size();
	if (size == 0) {
		return 0.;
	}
	double sum{};
	for (auto &p : particles) {
		// used to correct for higher distances due to passing periodic boundaries
		std::array<double, 3> correction = {0., 0., 0.};
		if (p.isPassedPeriodicX()) {
			p.setPassedPeriodic(0);
			correction[0] = particles.getLenDim()[0];
		}
		if (p.isPassedPeriodicY()) {
			p.setPassedPeriodic(1);
			correction[1] = particles.getLenDim()[1];
		}
		if (p.isPassedPeriodicZ()) {
			p.setPassedPeriodic(2);
			correction[2] = particles.getLenDim()[2];
		}
		double dist = ArrayUtils::L2Norm((p.getX() - correction) - p.getOldX());
		sum += dist * dist;
		p.setOldX(p.getX());
	}
	return sum / static_cast<double>(size);
}

void Thermodynamical::writeStatistics(ParticleContainer &container, int iteration) {
	auto &particles = static_cast<LinkedCellContainer &>(container);

	std::ofstream file_diff{folder_path + "diffusion_" + filename, std::ios::app};
	file_diff << iteration << "," << calculateDiffusion(particles) << "\n";

	std::ofstream file_rdf{folder_path + "RDF_" + filename, std::ios::app};

	int intervals = std::floor(max_distance / delta_r);
	std::vector<int> molCounts{};
	molCounts.assign(intervals, 0);

	for (auto it = particles.pair_begin(); it != particles.pair_end(); ++it) {
		auto[p1, p2] = *it;

		double dist = ArrayUtils::L2Norm(p2.get().getX() - p1.get().getX());
		int binNumber = std::floor(dist / delta_r);
		molCounts[binNumber] = molCounts[binNumber] + 1;
	}

	for (int i = 0; i < (int) molCounts.size(); i++) {
		// (ri + δr)^3 - ri^3 = (i*δr + δr)^3 - (i*δr)^3 = δr^3 * (3*i^2 + 3i +1)
		double local_density = molCounts[i] / (FOUR_THIRDS_PI * delta_r * delta_r * delta_r * (3 * i * (i + 1) + 1));
		file_rdf << iteration << "," << i * delta_r << ";" << (i + 1) * delta_r << "," << local_density << "\n";
	}

}

}