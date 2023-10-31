#pragma once

#include "thermal_common.hpp"
#include "thermal_solver.hpp"
#include "thermal_data.hpp"

class ThermalGui {
	
public:

	void autoAdjustDisplayRange(const Vec& _values);

	void draw(
		unsigned int vertex_count,
		ThermalSolver& solver,
		ThermalVars_s& thermalVars,
		const ThermalData& data,
		const ThermalObjects& objs
	);

	// display
	int		sourceVertexId = 0;
	int		visualization = 2;
	bool	customNormalization = false;
	float	normalizationFactor = 300.0 * kelvinUnitFactor;
	float	displayRangeMin = 0.0;
	float	displayRangeMax = 1.0;
	bool	enableAutoAdjust = true;
	float	clipDistance = 0.0;
	bool	backfaceCulling = false;

	// misc
	bool	showProperties = true;
	bool	showStatistics = true;

private:
	// ...

};