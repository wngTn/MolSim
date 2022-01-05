#include "Thermodynamical.h"
#include "utils/ArrayUtils.h"

#include <utility>
#include <fstream>
#include <particleContainers/LinkedCellContainer.h>
#include <iostream>

namespace statistics {

    Thermodynamical::Thermodynamical(std::string file, double delta_r) : filename{std::move(file)}, delta_r{delta_r}, max_distance{50.} {
        std::ofstream f{filename};
        f << "Iteration,Diffusion";
        for(int i = 0; i*delta_r < max_distance; i++){
            f << ",Interval [" << i * delta_r << ";" << (i+1) * delta_r << "]";
        }
        f << "\n";
    }

    double Thermodynamical::calculateDiffusion(ParticleContainer& particles){
        if(particles.size() == 0){
            return 0.;
        }

        return -1;
    }

    void Thermodynamical::writeStatistics(ParticleContainer &container, int iteration) {
        auto particles = static_cast<LinkedCellContainer &>(container);

        std::ofstream file{filename, std::ios::app};
        file << iteration << "," << calculateDiffusion(particles);

        int intervals = std::floor(max_distance / delta_r);
        std::vector<int> molCounts{};
        molCounts.assign(intervals, 0);

        for (auto it = particles.pair_begin(); it != particles.pair_end(); ++it) {
            auto [p1, p2] = *it;

            double dist = ArrayUtils::L2Norm(p2.get().getX() - p1.get().getX());
            int binNumber = std::floor(dist/delta_r);
            molCounts[binNumber] = molCounts[binNumber] + 1;
        }

        for(int i = 0; i < (int) molCounts.size(); i++){
            // (ri + δr)^3 - ri^3 = (i*δr + δr)^3 - (i*δr)^3 = δr^3 * (3*i^2 + 3i +1)
            double local_density = molCounts[i] / (FOUR_THIRDS_PI * delta_r*delta_r*delta_r * (3*i*(i+1) + 1));
            file << "," << local_density;
        }
        file << "\n";

    }

}