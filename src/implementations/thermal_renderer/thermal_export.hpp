#pragma once

#include "thermal_renderer.hpp"
#include "thermal_solver.hpp"

void export_to_csv_point_values(const Vec& _values);
void export_to_csv_object_avg(const std::vector<ObjectStatistics_s>& thermalStatsArray);
void export_vtk(const Vec& _values);
