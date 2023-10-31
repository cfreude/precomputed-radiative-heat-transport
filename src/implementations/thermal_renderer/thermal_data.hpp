#pragma once

#include <tamashii/engine/scene/render_scene.hpp>
#include <tamashii/engine/render/render_backend_implementation.hpp>

#include "thermal_common.hpp"
#include "thermal_objects.hpp"

T_USE_NAMESPACE

class ThermalData {

public:
	void init(scene_s _scene, const ThermalObjects& _thermal_objects, unsigned int _vertex_count, unsigned int _triangle_count);
	void load(scene_s _scene, ThermalObjects& _thermal_objects, SCALAR _sky_minimum_kelvin = 0.0);
	void reset();
	void unload();

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	Vec	initialValueVector;
	Vec	currentValueVector;
	Vec	emissionVector;
	Vec	absorptionVector;

	Vec vertexAreaVector;
	VectorXi vertexTriangleCountVector;
	Vec triangleAreaVector;
	Vec fixedVarsVector;

	std::vector<ObjectStatistics_s> objectStatistics;
	void setObjectStatistics(const ThermalObjects& objects, const Vec& _values);

private:
	// ...
};