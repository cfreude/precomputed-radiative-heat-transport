#pragma once

#include <Eigen/Eigen>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include "rvk/rvk.hpp"

using namespace Eigen;

#define CHECK_EMPTY_SCENE(SCENE) if (SCENE.refModels.size() == 0) { spdlog::warn("Scene has no models."); return; }

typedef float SCALAR;
typedef Matrix<SCALAR, Eigen::Dynamic, 1> Vec;
typedef Matrix<SCALAR, Eigen::Dynamic, Eigen::Dynamic, RowMajor> Mat;

const SCALAR kelvinUnitFactor = 1e-3; // kilo kelvin
const SCALAR secondsUnitFactor = 1.0 / 3600.0; // hours
const SCALAR STEFAN_BOLTZMANN_CONST_RAW = 5.670374419e-8;
const SCALAR STEFAN_BOLTZMANN_CONST = STEFAN_BOLTZMANN_CONST_RAW / (pow(kelvinUnitFactor, 4.0) * pow(secondsUnitFactor, 3.0)); // W m ^ -2 K ^ -4 

constexpr char THERMAL_RENDERER_NAME[] = "Thermal Renderer";

SCALAR sky_value_to_kelvin(SCALAR _value, SCALAR _area, SCALAR _min);
SCALAR condistion_number(Mat _mat);

void printVramSize(rvk::LogicalDevice* _device);

template <typename T>
void logEigenBase(const std::string tag, const DenseBase<T>& v)
{
#ifndef RUNTIME_OPTIMIZED
	if (v.cols() < 40)
	{
		spdlog::debug("{}:\n{}", tag, v);
	}
	else
	{
		spdlog::debug("{}:", tag);
		spdlog::debug("\tmin: {}", v.minCoeff());
		spdlog::debug("\tmean: {}", v.mean());
		spdlog::debug("\tmax : {}", v.maxCoeff());
	}
#endif // !RUNTIME_OPTIMIZED
}

typedef struct ThermalVars_s {
	// simulation
	bool reset = false;
	bool recomputeTransport = false;
	int	timeSteps = 100;
	int remainingTimeSteps = 0;
	float simulationTime = 0.0f;
	int rayDepth = 0;
	int rayCount = 10000;
	int batchCount = 50;
} ThermalVars_s;

typedef struct ObjectStatistics_s {
	std::string											name;
	SCALAR												min;
	SCALAR												max;
	SCALAR												avg;
} ObjectStatistics_s;