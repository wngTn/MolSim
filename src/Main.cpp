#include "utils/MainUtils.h"

int main(int argc, char *argv[]) {
    MainUtils::initializeLogger();

    auto start_setup = std::chrono::steady_clock::now();

    // parse cmd line args and set static values accordingly
    MainUtils::get_arguments(argc, argv);

    // ------ setup ------ //
    MainUtils::parseXML();

    MainUtils::printConfig();

    auto particles = MainUtils::get_container();
    MainUtils::initializeParticles(*particles);

    auto io = MainUtils::get_io_type();
    auto calc = MainUtils::get_calculator();

    // TODO get frome XML
    config.nThermostat = 100;

    calc->setDim(config.DIM);
    calc->setDeltaT(config.delta_t);

    double current_time = config.start_time;
    int iteration = 0;
    // ------ end setup ------ //
    if(config.benchmarking){
        std::cout
                << "\u001b[31mYou have chosen the benchmark mode!\u001b[0m" << std::endl
                << "\u001b[31mElapsed setup time [microseconds]:\u001b[0m "
                << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_setup).count()
                << std::endl
                << "\u001b[31mIn total there are:\u001b[0m " << particles->size() << " particles." << std::endl;
    }

    spdlog::info(
            "Start calculating particles with\n\tIO type:\t\t{:<15}\n\tcalculator type:\t{:<15}\n\tend time:\t\t{:<15}\n\ttimestep:\t\t{:<15}",
            io->toString(), calc->toString(), config.end_time, config.delta_t);

    // ------ calculation ------ //
    auto start_calc = std::chrono::steady_clock::now();
    // initial setup
    // TODO setup temperature
    calc->calcX(*particles);
    particles->setup();
    calc->calcF(*particles);
    if (!config.benchmarking){
        io->write(*particles, "output", iteration);
    }
    // for this loop, we assume: current x, current f and current v are known
    while (current_time < config.end_time) {
        spdlog::info("Iteration {}: ", iteration);
        MainUtils::logParticle(*particles);


        calc->calcX(*particles);
        particles->setup();
        calc->calcF(*particles);
        calc->calcV(*particles);

        MainUtils::logParticle(*particles);

        iteration++;

        // todo apply thermostat stuff using Thermostat::applyTemperature()

        if (!config.benchmarking && iteration % config.writeFrequency == 0) {
            particles->cleanup();
            // setup after cleanup needed to validate pointers for calcX
            particles->setup();
            // uses abstract write method overwritten by specific IO method
            io->write(*particles, config.output_file, iteration);
        }

        current_time += config.delta_t;
    }
    // ------ end calculation ------ //
    if(config.benchmarking){
        auto runtimeDuration =
                std::chrono::duration_cast<std::chrono::milliseconds>
                        (std::chrono::steady_clock::now() - start_calc).count();
        double mups = static_cast<double>(particles->size()) * (config.end_time/config.delta_t) /
                      (static_cast<double>(runtimeDuration) / 1000);
        std::cout
                << "\u001b[31mElapsed calculation time [milliseconds]:\u001b[0m "
                << runtimeDuration
                << std::endl;
        if (mups > 1000000) {
            std::cout<<"\u001b[31mMMUPS/s:\u001b[0m "<<mups/1000000<<std::endl;
        } else {
            std::cout<<"\u001b[31mMUPS/s:\u001b[0m "<<mups<<std::endl;
        }
    } else{
        std::cout << "All files have been written!" << std::endl;
        spdlog::info("All files have been written!");
    }

    return 0;
}