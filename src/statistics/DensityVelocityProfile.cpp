#include "DensityVelocityProfile.h"

#include "particleContainers/LinkedCellContainer.h"
#include "utils/ArrayUtils.h"

#include <fstream>
#include <utility>
#include <iostream>

namespace statistics {

    DensityVelocityProfile::DensityVelocityProfile(std::string filename, int noBins) : filename(std::move(filename)),
                                                                                              noBins(noBins) {
        std::ofstream file{filename};
        file << "Iteration,Bin,Density,Velocity X,Velocity Y,Velocity Z\n";
    }


    void DensityVelocityProfile::writeStatistics(ParticleContainer &container, int iteration) {
        // idk how this would work for DirectSum? dimensionLength with fixed values? idk
        auto particles = static_cast<LinkedCellContainer &>(container);

        double totalSize = particles.getLenDim()[0];
        double binSize = totalSize / noBins;
        std::vector<std::vector<Particle *>> bins;
        bins.reserve(noBins);
        bins.assign(noBins, {});

        for (auto &p: particles) {
            int binIndex = std::floor(p.getX()[0] / binSize);
            bins[binIndex].push_back(&p);
        }
        // open for appending
        std::ofstream file{filename, std::ios::app};

        int binNumber = 0;
        for (auto &bin: bins) {
            if (bin.empty()) {
                file << iteration << "," << binNumber << ",-,-,-,-\n";
                binNumber++;
                continue;
            }
            auto noParts = static_cast<double>(bin.size());

            auto totalVel = std::accumulate(bin.begin(), bin.end(), std::array<double, 3>{0., 0., 0.},
                                            [](auto acc, auto *p) { return p->getV() + acc; });
            auto avgVel = std::array<double, 3>{1. / noParts, 1. / noParts, 1. / noParts} * totalVel;

            // TODO check if computing the correct thing?
            // currently density = number of particles in bin & velocity = totalVel/noParts for each dimension
            file << iteration << "," << binNumber << "," << noParts << "," << avgVel[0] << "," << avgVel[1] << ","
                 << avgVel[2] << "\n";
            binNumber++;
        }

    }

} // end namespace statistics