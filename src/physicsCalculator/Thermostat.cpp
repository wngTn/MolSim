#include "Thermostat.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

#include <algorithm>


Thermostat::Thermostat(double initialTemperature, double targetTemperature, double maxDeltaTemperature)
        : initialTemperature(initialTemperature), targetTemperature(targetTemperature),
          maxDeltaTemperature(maxDeltaTemperature) {}


double Thermostat::calculateKineticEnergy(ParticleContainer &particles) {
    double totalEnergy = 0;
    for(auto &p: particles){
        totalEnergy += p.getM() * (ArrayUtils::dotProduct(p.getV(), p.getV()));
    }
    return totalEnergy * 0.5;
}

double Thermostat::calculateCurrentTemp(ParticleContainer &particles){
    return 2 * calculateKineticEnergy(particles) / static_cast<int>(particles.size()) / particles.dimensions();
}

void Thermostat::scaleVelocities(ParticleContainer &particles, double beta) {
    for(auto &p : particles){
        p.setV(beta * p.getV());
    }
}

void Thermostat::applyTemperature(ParticleContainer &particles) const{
    double temp = calculateCurrentTemp(particles);
    double newTemp;
    if(temp > targetTemperature){
        newTemp = std::max(targetTemperature, temp-maxDeltaTemperature);

    }else if (temp < targetTemperature){
        newTemp = std::min(targetTemperature, temp+maxDeltaTemperature);
    }else{
        // temperature already perfect, no need to scale
        return;
    }
    double beta = sqrt(newTemp/temp);
    scaleVelocities(particles, beta);
}

// check if _all_ velocities are zero, if yes apply BrownianMotion using init temp
// ofc we could check that during particle generation and pass that information along etc.
// but this is executed once (1) at startup so who cares
// also scale velocities to init temperature
void Thermostat::setupTemperature(ParticleContainer &particles) const{
    // this is bc mass is often 1 -> sqrt(1) easy, /1 easy as well -> only 1 complex sqrt calculation
    // works bc sqrt(a/b) = sqrt(a)/sqrt(b)
    double tempFactor = sqrt(initialTemperature);
    // if all velocities are zero
    if(std::all_of(particles.begin(), particles.end(), [](auto& p){
        return p.getV()[0]==0 && p.getV()[1]==0 && p.getV()[2]==0;})){
        for(auto &p : particles){
            p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(tempFactor/sqrt(p.getM()), particles.dimensions()));
        }
    }
    scaleVelocities(particles, sqrt(initialTemperature / calculateCurrentTemp(particles)));
}

