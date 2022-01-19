#include <iostream>
#include <cfloat>

#include "XMLReader.h"


XMLReader::XMLInfo XMLReader::readFile(const std::string& s) {
    auto sim = simulation(s,  xml_schema::flags::dont_validate);

    XMLInfo info{};

    if(sim->container().type() == containertype_t::linkedcell){
        info.linkedcell = true;
        info.rCut = sim->container().rCut().get();
        info.linkedCellSize = {sim->container().domainSizeX().get(),
                               sim->container().domainSizeY().get(),
                               sim->container().domainSizeZ().get()};
        auto borderType = sim->container().borderType().get();
        std::array<LinkedCellContainer::Border, 6> boundaryConds = {};
        // this is ugly but idk how to iterate the borderType thing
        initializeBorderType(0,borderType.left(), boundaryConds);
        initializeBorderType(1,borderType.right(), boundaryConds);
        initializeBorderType(2,borderType.upper(), boundaryConds);
        initializeBorderType(3,borderType.lower(), boundaryConds);
        if(borderType.front().present()){
            initializeBorderType(4,borderType.front().get(), boundaryConds);
        }
        if(borderType.back().present()){
            initializeBorderType(5,borderType.back().get(), boundaryConds);
        }
        info.boundaryConditions = boundaryConds;
    }else{
        info.linkedcell = false;
    }

    switch(sim->calculator().type()){
        case calculatortype_t::smoothed_lennardjones:
            info.smoothed = true;
            info.rl = sim->calculator().rl().get();
            [[fallthrough]];
        case calculatortype_t::lennardjones:
            info.epsilon = sim->calculator().epsilon().get();
            info.sigma = sim->calculator().sigma().get();
            info.brownianMotionMean = sim->calculator().brownianMotion().get();
            if(sim->calculator().gravityFactor().present()){
                info.gravityFactor = {sim->calculator().gravityFactor()->x(), sim->calculator().gravityFactor()->y(), sim->calculator().gravityFactor()->z()};
            }else{
                info.gravityFactor = {0.,0.,0.};
            }
            info.calculatorType = PhysicsCalc::lennardJones;
            if(sim->calculator().baseForceTime().present()){
                info.resetBaseForce = true;
                info.baseForceReset = std::ceil(sim->calculator().baseForceTime().get() / sim->delta_t());
            }
            if(sim->calculator().rZero().present() && sim->calculator().stiffnessConstant().present()){
                info.membrane = true;
                info.rZero = sim->calculator().rZero().get();
                info.stiffnessConstant = sim->calculator().rZero().get();
            }else{
                info.membrane = false;
                info.rZero = 0;
                info.stiffnessConstant = 0;
            }
            break;
        case calculatortype_t::gravitation:
            info.calculatorType = PhysicsCalc::gravitation;
            break;
    }


    if(sim->thermostat().present()){
        info.useThermostat = true;
        info.n_thermostat = sim->thermostat()->nThermostat();
        info.t_init = sim->thermostat()->Tinit();
        info.t_target = sim->thermostat()->Ttarget().present() ? sim->thermostat()->Ttarget().get() : info.t_init;
        info.delta_temp = sim->thermostat()->deltaTemp().present() ? sim->thermostat()->deltaTemp().get() : DBL_MAX;

        info.excludeY = sim->thermostat()->excludeY().present() && sim->thermostat()->excludeY().get();
    }else{
        info.useThermostat = false;
    }

    info.delta_t = sim->delta_t();
    info.t_end = sim->t_end();
    info.writeFrequency = sim->writeFrequency();
    if(sim->random().present()){
        info.random = sim->random().get();
    } else {
        info.random = false;
    }

    switch(sim->outputWriter()){
        case outputwriter_t::vtk:
            info.outputWriterType = IOWriter::vtk;
            break;
        case outputwriter_t::xyz:
            info.outputWriterType = IOWriter::xyz;
            break;
    }

    if(sim->statistics().present()){
        info.useStatistics = true;
        info.statsFile = sim->statistics().get().file();
        info.statsFrequency = sim->statistics().get().frequency();
        switch(sim->statistics()->type()){
            case statistics_type_t::thermodynamical:
                info.statsType = StatisticsLogger::thermodynamic;
                info.noBins = 0;
                info.delta_r = sim->statistics()->deltaR().get();
                break;
            case statistics_type_t::densityVelocity:
                info.statsType = StatisticsLogger::densityVelocityProfile;
                info.noBins = sim->statistics()->noBins().get();
                info.delta_r = 0;
                break;
        }
    }

    info.inputFiles = std::vector<std::string>{};
    for(auto& inpf : sim->inputFile()){
        info.inputFiles.push_back(inpf);
    }

    info.outputfile = sim->outputFile();

    if(sim->checkpointInput().present()){
        info.checkpointInput = sim->checkpointInput().get();
    }else{
        info.checkpointInput = "";
    }

    if(sim->checkpointOutput().present()){
        info.checkpointOutput = sim->checkpointOutput().get();
    }else{
        info.checkpointOutput = "";
    }

    info.generatorInputFiles = std::vector<std::string>{};
    for(auto& ginpf : sim->generatorFile()){
        info.generatorInputFiles.push_back(ginpf);
    }

    auto genInfos = std::vector<ParticleGenerator::ShapeInfo>{};

    for(auto& gi : sim->generatorInfo()){
        insertGeneratorInfo(genInfos, gi);
    }
    info.generatorInfos = genInfos;

    if(sim->parallelization().present()){
        switch(sim->parallelization().get()){
            case parallel_t::subdomain:
                info.parallelization_strat = LinkedCellContainer::subDomain;
                break;
            case parallel_t::primitiveX:
                info.parallelization_strat = LinkedCellContainer::primitiveX;
                break;
            case parallel_t::primitiveY:
                info.parallelization_strat = LinkedCellContainer::primitiveY;
                break;
            case parallel_t::primitiveZ:
                info.parallelization_strat = LinkedCellContainer::primitiveZ;
                break;
            case parallel_t::none:
                info.parallelization_strat = LinkedCellContainer::naught;
                break;
        }
    }

    return info;
}

void XMLReader::initializeBorderType(int index, border_single_t& type, std::array<LinkedCellContainer::Border, 6>& boundaryConds) {
    switch (type) {
        case border_single_t::periodic:
            boundaryConds[index] = LinkedCellContainer::periodic;
            break;
        case border_single_t::reflective:
            boundaryConds[index] = LinkedCellContainer::reflective;
            break;
        case border_single_t::outflow:
            boundaryConds[index] = LinkedCellContainer::outflow;
            break;
    }
}

void XMLReader::insertGeneratorInfo(std::vector<ParticleGenerator::ShapeInfo>& genInfos, generator_info_t &info) {
    ParticleGenerator::ShapeInfo shapeInfo{};
    shapeInfo.mass = info.mass();
    shapeInfo.distance = info.distance();
    shapeInfo.DIM = info.dim();
    shapeInfo.brownianFactor = info.brownianFactor();

    shapeInfo.pos = {info.x(), info.y(), info.z()};
    shapeInfo.vel = {info.v1(), info.v2(), info.v3()};

    switch(info.type()){
        case geometric_t::cuboid:
            shapeInfo.N = {info.n1().get(), info.n2().get(), info.n3().get()};
            shapeInfo.type = ParticleGenerator::cuboid;
            break;
        case geometric_t::sphere:
            shapeInfo.radius = info.radius().get();
            shapeInfo.type = ParticleGenerator::sphere;
            break;
    }

    shapeInfo.sigma = info.sigma();
    shapeInfo.epsilon = info.epsilon();

    if(info.behaviour().present()){
        switch (info.behaviour().get()){
            case behaviour_t::membrane:
                shapeInfo.behaviour = ParticleGenerator::membrane;
                break;
            case behaviour_t::immovable:
                shapeInfo.behaviour = ParticleGenerator::immovable;
                break;
            case behaviour_t::normal:
            default:
                shapeInfo.behaviour = ParticleGenerator::normal;
                break;
        }
    }

    if(info.baseForce().present()){
        shapeInfo.baseForce = std::array<double,3>{info.baseForce()->x(),info.baseForce()->y(), info.baseForce()->z()};
    }

    shapeInfo.specialParticles = {};
    for(auto& sp : info.special_particle()){
        auto pos = std::array<int,3>{sp.position().x(), sp.position().y(), sp.position().z()};
        auto vel = shapeInfo.vel;
        auto force = std::array<double,3>{0.,0.,0.};
        auto mass = shapeInfo.mass;
        bool immovable = shapeInfo.behaviour == ParticleGenerator::immovable;
        bool membrane = shapeInfo.behaviour == ParticleGenerator::membrane;
        if(sp.vel().present()){
            vel = {sp.vel()->x(),sp.vel()->y(),sp.vel()->z()};
        }
        if(sp.force().present()){
            force = {sp.force()->x(),sp.force()->y(),sp.force()->z()};
        }
        if(sp.mass().present()){
            mass = sp.mass().get();
        }
        if(sp.immovable().present()){
            immovable = sp.immovable().get();
        }
        if(sp.membrane().present()){
            membrane = sp.immovable().get();
        }
        shapeInfo.specialParticles.emplace_back(pos,force,vel,mass, immovable, membrane);
    }

    genInfos.push_back(shapeInfo);
}


