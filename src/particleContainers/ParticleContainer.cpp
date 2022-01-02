#include "ParticleContainer.h"

void to_json(json& j, const ParticleContainer& container){
    json o;
    for(auto &p : container){
        o.push_back(p);
    }
    j = o;
}

void from_json(const nlohmann::json&j, ParticleContainer& container){
    for (auto & it : j) {
        container.push_back(it.get<Particle>());
    }
}

void ParticleContainer::reset_base_force(){
    std::array<double,3> f{0.,0.,0.};
    for(auto &p : *this){
        p.setBaseForce(f);
    }
}

