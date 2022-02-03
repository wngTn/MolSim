#pragma once

#include "StatisticsLogger.h"

namespace statistics {

class DensityVelocityProfile : public StatisticsLogger {

public:
	DensityVelocityProfile(std::string filename, int noBins);

	void writeStatistics(ParticleContainer &container, int iteration) override;

private:
	std::string filename;
	int noBins;
};

} // end namespace statistics