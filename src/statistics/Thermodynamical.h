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
    // copied from std::numbers which doesn't work on older compiler versions
    constexpr double pi = 3.141592653589793238462643383279502884L;
    constexpr double FOUR_THIRDS_PI = pi * (4./3.);
}