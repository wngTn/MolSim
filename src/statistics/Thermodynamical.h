#pragma once

#include "StatisticsLogger.h"

namespace statistics {

    class Thermodynamical : public StatisticsLogger {
    public:
        Thermodynamical(std::string filename, double delta_r);

    private:

        void writeStatistics(ParticleContainer &container, int iteration) override;

    private:
        std::string filename;
        double delta_r;
        double max_distance;

        static double calculateDiffusion(ParticleContainer &particles);
    };

    constexpr double FOUR_THIRDS_PI = std::numbers::pi * (4./3.);
}