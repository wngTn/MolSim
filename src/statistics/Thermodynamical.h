#pragma once

#include "StatisticsLogger.h"

namespace statistics {

    class Thermodynamical : public StatisticsLogger {
    public:
        Thermodynamical(std::string filename, double delta_r);

    private:

        void writeStatistics(ParticleContainer &container, int iteration) override;

    private:
        double delta_r;
    };
}