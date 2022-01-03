#include "Thermostat.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

#include <algorithm>
#include <iostream>


Thermostat::Thermostat(double initialTemperature, double targetTemperature, double maxDeltaTemperature, bool exludeY)
        : initialTemperature(initialTemperature), targetTemperature(targetTemperature),
          maxDeltaTemperature(maxDeltaTemperature), excludeY(exludeY) {}

double Thermostat::calculateMeanYVelocity(ParticleContainer &container) {
    double totalYVel = std::accumulate(container.begin(), container.end(), 0, [](double acc, auto&p){return acc + p.getX()[1];});
    return totalYVel/static_cast<int>(container.size());
}


double Thermostat::calculateKineticEnergy(ParticleContainer &particles, bool exclY) {
    double totalEnergy = 0;
    double meanYVel = 0.;
    if(exclY){
        meanYVel = calculateMeanYVelocity(particles);
    }
    for(auto &p: particles){
        if(!p.immovable){
            std::array<double,3> v{};
            if(exclY){
                v = p.getV() - std::array<double,3>{0.,meanYVel,0.};
            }else{
                v = p.getV();
            }
            totalEnergy += p.getM() * (ArrayUtils::dotProduct(v, v));
        }
    }
    return totalEnergy * 0.5;
}

double Thermostat::calculateCurrentTemp(ParticleContainer &particles, bool exclY){
    return 2 * calculateKineticEnergy(particles, exclY) / static_cast<int>(particles.size()) / particles.dimensions();
}

void Thermostat::scaleVelocities(ParticleContainer &particles, double beta, bool exclY) {
    if(exclY){
        std::array<double,3> b = {beta, 1.0, beta};
        for(auto &p : particles){
            if(!p.immovable){
                p.setV(b * p.getV());
            }
        }
        return;
    }
    for(auto &p : particles){
        if(!p.immovable){
            p.setV(beta * p.getV());
        }
    }
}

void Thermostat::applyTemperature(ParticleContainer &particles) const{
    double temp = calculateCurrentTemp(particles, excludeY);
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
    scaleVelocities(particles, beta, excludeY);
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
            if(!p.immovable){
                std::array<double,3> exclY = {1.0,excludeY?0.0:1.0,1.0};
                p.setV(p.getV() + (exclY * maxwellBoltzmannDistributedVelocity(tempFactor/sqrt(p.getM()), particles.dimensions())));
            }
        }
    }
    scaleVelocities(particles, sqrt(initialTemperature / calculateCurrentTemp(particles, excludeY)), excludeY);
}


