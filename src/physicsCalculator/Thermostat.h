#pragma once

#include "particleContainers/ParticleContainer.h"

#include <cfloat>



    class Thermostat {

    public:
        Thermostat() = default;

        Thermostat(double initialTemperature, double targetTemperature, double maxDeltaTemperature);

        void setupTemperature(ParticleContainer &particles) const;

        void applyTemperature(ParticleContainer &particles) const;


    private:

        static double calculateKineticEnergy(ParticleContainer &particles);
        static double calculateCurrentTemp(ParticleContainer &particles);
        static void scaleVelocities(ParticleContainer &particles, double beta);

        double initialTemperature;
        double targetTemperature;
        double maxDeltaTemperature;
    };

