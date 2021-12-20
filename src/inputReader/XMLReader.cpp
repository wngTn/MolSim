#include <iostream>
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

    if(sim->calculator().type() == calculatortype_t::lennardjones){
        info.epsilon = sim->calculator().epsilon().get();
        info.sigma = sim->calculator().sigma().get();
        info.brownianMotionMean = sim->calculator().brownianMotion().get();
        if(sim->calculator().gravityFactor().present()){
            info.gravityFactor = sim->calculator().gravityFactor().get();
        }else{
            info.gravityFactor = 0.0;
        }
        info.calculatorType = PhysicsCalc::lennardJones;
    }else{
        info.calculatorType = PhysicsCalc::gravitation;
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

    info.inputFiles = std::vector<std::string>{};
    for(auto& inpf : sim->inputFile()){
        info.inputFiles.push_back(inpf);
    }

    info.outputfile = sim->outputFile();

    info.generatorInputFiles = std::vector<std::string>{};
    for(auto& ginpf : sim->generatorFile()){
        info.generatorInputFiles.push_back(ginpf);
    }

    auto genInfos = std::vector<ParticleGenerator::ShapeInfo>{};

    for(auto& gi : sim->generatorInfo()){
        insertGeneratorInfo(genInfos, gi);
    }
    info.generatorInfos = genInfos;

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

    genInfos.push_back(shapeInfo);
}


