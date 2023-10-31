#include "thermal_common.hpp"

#include <Eigen/Core>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

using namespace Eigen;

SCALAR sky_value_to_kelvin(SCALAR _value, SCALAR _area, SCALAR _min) // value already in W / m^2 
{
	// area not needed as sky values are already in W/m2
	SCALAR kelvin = pow(_value / STEFAN_BOLTZMANN_CONST_RAW, 1.0 / 4.0);
	kelvin = std::max(kelvin, _min); // clamp
	return kelvin * kelvinUnitFactor;
}

// from https://stackoverflow.com/questions/33575478/how-can-you-find-the-condition-number-in-eigen
SCALAR condistion_number(Mat _mat)
{
	JacobiSVD<Mat> svd(_mat);
	Vec sigma = svd.singularValues();
	return sigma.maxCoeff() / sigma.minCoeff();
}

void printVramSize(rvk::LogicalDevice* _device)
{
	VkPhysicalDeviceMemoryBudgetPropertiesEXT props = _device->getMemoryBudgetProperties();
	for(int i=0;i<16; i++)
		spdlog::info("heap #{}, budget: {}, usage: {}", i, props.heapBudget[i], props.heapUsage[i]);
}