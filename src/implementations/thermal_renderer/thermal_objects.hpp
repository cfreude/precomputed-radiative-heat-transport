#pragma once

#include "thermal_common.hpp"
#include <string>

class ThermalObjects {

public:

	std::vector<std::string> name;

	// thermal values
	Vec	kelvin;
	Vec	thickness;
	Vec	density;
	Vec	heatCapacity;
	Vec	heatConductivity;

	// radiation values	
	Vec	diffuseReflectance;
	Vec	specularReflectance;
	Vec	absorption;

	// simulation flags
	std::vector<bool> diffuseEmission;
	std::vector<bool> temperatureFixed;
	std::vector<bool> traceable;

	std::vector<unsigned int> vertexCount;
	std::vector<unsigned int> vertexOffset;

	void reserve(unsigned int _size);
	void resize(unsigned int _size);
	void clear();
	void print(unsigned int _index) const;
	const char * getUiStr(unsigned int index) const;

	unsigned int count = 0;

private:

	// ...

};