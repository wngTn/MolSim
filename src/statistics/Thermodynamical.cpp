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

    double Thermodynamical::calculateDiffusion(ParticleContainer& container){
        auto& particles = static_cast<LinkedCellContainer &>(container);
        auto size = particles.size();
        if(size == 0){
            return 0.;
        }
        double sum{};
        for(auto& p : particles){
            // used to correct for higher distances due to passing periodic boundaries
            std::array<double,3> correction = {0.,0.,0.};
            if(p.isPassedPeriodicX()){
                p.setPassedPeriodic(0);
                correction[0] = particles.getLenDim()[0];
            }
            if(p.isPassedPeriodicY()){
                p.setPassedPeriodic(1);
                correction[1] = particles.getLenDim()[1];
            }
            if(p.isPassedPeriodicZ()){
                p.setPassedPeriodic(2);
                correction[2] = particles.getLenDim()[2];
            }
            double dist = ArrayUtils::L2Norm((p.getX() - correction)  - p.getOldX());
            sum += dist * dist;
            p.setOldX(p.getX());
        }
        return sum/static_cast<double>(size);
    }

    void Thermodynamical::writeStatistics(ParticleContainer &container, int iteration) {
        auto& particles = static_cast<LinkedCellContainer &>(container);

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