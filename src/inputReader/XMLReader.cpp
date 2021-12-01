#include "XMLReader.h"
#include "simulation.hxx"

XMLReader::XMLInfo XMLReader::readFile(const std::string& s) {
    // flags?
    auto sim = simulation(s,  xml_schema::flags::dont_validate);

    XMLInfo info{};

    if(sim->container_type() == containertype_t::linkedcell){
        info.linkedcell = true;
        auto borderType = sim->containerinfo().get().borderType();

        std::array<LinkedCellContainer::Border, 6> boundaryConds = {};
        switch (borderType.left()) {
            case border_single_t::cyclic:
                boundaryConds[0] = LinkedCellContainer::cyclic;
                break;
            case border_single_t::reflective:
                boundaryConds[0] = LinkedCellContainer::reflective;
                break;
            case border_single_t::outflow:
                boundaryConds[0] = LinkedCellContainer::outflow;
                break;
        }
    }

    info.delta_t = sim->delta_t();
    //TODO XML READER
    return info;
}


