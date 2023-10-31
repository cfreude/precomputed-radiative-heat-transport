#include "thermal_objects.hpp"
#include <iomanip>

void ThermalObjects::reserve(unsigned int _size)
{
	name.reserve(_size);

	// thermal values
	kelvin.resize(_size);
	thickness.resize(_size);
	density.resize(_size);
	heatCapacity.resize(_size);
	heatConductivity.resize(_size);

	// radiation values	
	diffuseReflectance.resize(_size);
	specularReflectance.resize(_size);
	absorption.resize(_size);

	// simulation flags
	diffuseEmission.reserve(_size);
	temperatureFixed.reserve(_size);
	traceable.reserve(_size);

	vertexCount.reserve(_size);
	vertexOffset.reserve(_size);
}

void ThermalObjects::resize(unsigned int _size)
{
	name.resize(_size);

	// thermal values
	kelvin.resize(_size);
	thickness.resize(_size);
	density.resize(_size);
	heatCapacity.resize(_size);
	heatConductivity.resize(_size);

	// radiation values	
	diffuseReflectance.resize(_size);
	specularReflectance.resize(_size);
	absorption.resize(_size);

	// simulation flags
	diffuseEmission.resize(_size);
	temperatureFixed.resize(_size);
	traceable.resize(_size);

	vertexCount.resize(_size);
	vertexOffset.resize(_size);
}

void ThermalObjects::clear()
{
	name.clear();

	// thermal values
	kelvin.setZero();
	thickness.setZero();
	density.setZero();
	heatCapacity.setZero();
	heatConductivity.setZero();

	// radiation values	
	diffuseReflectance.setZero();
	specularReflectance.setZero();
	absorption.setZero();

	// simulation flags
	diffuseEmission.clear();
	temperatureFixed.clear();
	traceable.clear();

	vertexCount.clear();
	vertexOffset.clear();
}

void ThermalObjects::print(unsigned int _index) const
{
	spdlog::debug("##### Object: {}", name[_index]);
	spdlog::debug("- - -");
	spdlog::debug("kelvin: {}", kelvin[_index]);
	spdlog::debug("temperature_fixed: {}", temperatureFixed[_index]);
	spdlog::debug("---");
	spdlog::debug("diffuse_reflectance: {}", diffuseReflectance[_index]);
	spdlog::debug("specular_reflectance: {}", specularReflectance[_index]);
	spdlog::debug("absorption: {}", absorption[_index]);
	spdlog::debug("- - -");
	spdlog::debug("diffuse_emission: {}", diffuseEmission[_index]);
	spdlog::debug("traceable: {}", traceable[_index]);
	spdlog::debug("---");
	spdlog::debug("thickness: {}", thickness[_index]);
	spdlog::debug("density: {}", density[_index]);
	spdlog::debug("heat_capacity: {}", heatCapacity[_index]);
	spdlog::debug("heat_conductivity: {}", heatConductivity[_index]);
	spdlog::debug("#####");
}

const char* ThermalObjects::getUiStr(unsigned int index) const
{
	std::ostringstream ss;
	ss << std::setw(4) << std::setfill('0') << std::setprecision(2) << std::fixed;
	ss << "diffRefl: " << diffuseReflectance[index] << " | ";
	ss << "specRefl: " << specularReflectance[index] << " | ";
	ss << name[index];
	return ss.str().c_str();
}