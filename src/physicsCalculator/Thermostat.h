#pragma once

#include "particleContainers/ParticleContainer.h"

#include <cfloat>



    class Thermostat {

    public:
        Thermostat() = default;

        Thermostat(double initialTemperature, double targetTemperature, double maxDeltaTemperature);

        void setupTemperature(ParticleContainer &particles);

        void applyTemperature(ParticleContainer &particles);


    private:

        double calculateKineticEnergy(ParticleContainer &particles);
        double calculateCurrentTemp(ParticleContainer &particles);
        void scaleVelocities(ParticleContainer &particles, double beta);

        double initialTemperature;
        double targetTemperature;
        double maxDeltaTemperature;
    };

