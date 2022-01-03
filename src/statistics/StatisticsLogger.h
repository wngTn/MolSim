#pragma once

#include "particleContainers/ParticleContainer.h"

class StatisticsLogger {
public:
    enum statisticstype{
        densityVelocityProfile,
        thermodynamic
    };

    StatisticsLogger() = default;
    StatisticsLogger(const StatisticsLogger &) = default;

    virtual void writeStatistics(ParticleContainer& container, int iteration) = 0;
};


