#include "thermal_gui.hpp"

#include <math.h>
#include <imgui.h>

#include <tamashii/engine/common/common.hpp>

#include "thermal_export.hpp"


void ThermalGui::autoAdjustDisplayRange(const Vec& _values)
{
	if (enableAutoAdjust)
	{
		displayRangeMin = _values.minCoeff();
		displayRangeMax = _values.maxCoeff();
	}
}

void ThermalGui::draw(
	unsigned int vertex_count,
	ThermalSolver& solver,
	ThermalVars_s& thermalVars,
	const ThermalData& data,
	const ThermalObjects& objs)
{ 

#ifdef DISABLE_GUI
	assert(false);// , "drawUI called!");
#endif // DISABLE_GUI

	if (ImGui::Begin("Settings", NULL, 0))
	{
		ImGui::Separator();
		ImGui::Text("Display:");
		ImGui::Combo("Mode##vis", &visualization, "Emission to all\0Absorption from all\0Value\0");
		if (visualization != 2)
		{
			if (ImGui::InputInt("Source Vertex Index", &sourceVertexId, 1, 1))
				sourceVertexId = std::clamp<int>(sourceVertexId, 0, vertex_count - 1);
			ImGui::Text("Kelvin: %.2f", data.currentValueVector[sourceVertexId] / kelvinUnitFactor);
		}
		if (ImGui::InputFloat("Range Min", &displayRangeMin, 0.01, 1.0, "%.9f")) {
			displayRangeMin = std::max<SCALAR>(0.0, displayRangeMin);
		}
		if (ImGui::InputFloat("Range Max", &displayRangeMax, 0.01, 1.0, "%.9f")) {
			displayRangeMax = std::max<SCALAR>(0.0, displayRangeMax);
		}
		if (ImGui::Button("Adjust to Data")) {
			autoAdjustDisplayRange(data.currentValueVector);
		}
		ImGui::SameLine();
		ImGui::Checkbox("Enable Auto Adjust", &enableAutoAdjust);
		ImGui::Separator();
		ImGui::Text("Simulation:");
		ImGui::Combo("Mode##sim", &solver.mode, "Kelvin\0Radiance\0");
		if (solver.mode == 0)
		{
			ImGui::Checkbox("Compute Steady State", &solver.compute_steady_state);
			if (solver.compute_steady_state)
			{
				if (ImGui::InputFloat("Solver Step", &solver.step_size, 1.0, 0.01, "%.3f"))
					solver.step_size = std::max<SCALAR>(0.0, solver.step_size);
			}
			else
			{
				if (ImGui::InputFloat("Time Step", &solver.step_size, 1.0, 0.01, "%.3f"))
					solver.step_size = std::max<SCALAR>(0.0, solver.step_size);
				if (ImGui::InputInt("Time Step Count", &thermalVars.timeSteps, 1, 10))
					thermalVars.timeSteps = std::max<int>(1, thermalVars.timeSteps);
			}
		}
		if (ImGui::Button("Run"))
		{
			spdlog::info("Running...");
			thermalVars.remainingTimeSteps = thermalVars.timeSteps;
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##solver")) {
			thermalVars.reset = true;
		}
		if (!solver.compute_steady_state) {
			ImGui::SameLine();
			ImGui::Text("Time: %.2f sec.", thermalVars.simulationTime / secondsUnitFactor);
		}
		ImGui::Separator();
		ImGui::Text("Transport:");
		if (ImGui::InputInt("Ray Depth", &thermalVars.rayDepth, 1, 1)) {
			thermalVars.rayDepth = std::clamp<int>(thermalVars.rayDepth, 0, thermalVars.rayDepth);
		}
		if (ImGui::InputInt("Ray Count", &thermalVars.rayCount, 1, 1)) {
			thermalVars.rayCount = std::clamp<int>(thermalVars.rayCount, 1, thermalVars.rayCount);
		}
		if (ImGui::InputInt("Batch Count", &thermalVars.batchCount, 1, 1)) {
			thermalVars.rayCount = std::clamp<int>(thermalVars.rayCount, 1, thermalVars.rayCount);
		}
		if (ImGui::Button("Recompute Transport")) {
			thermalVars.recomputeTransport = true;
		}
		ImGui::Separator();
		ImGui::Text("Misc:");
		ImGui::SliderFloat("Clip Distance", &clipDistance, 0.0, 100.0);
		ImGui::Checkbox("Backface Distance", &backfaceCulling);
		ImGui::Separator();
		ImGui::Text("Properties:");
		ImGui::Checkbox("Show", &showProperties);
		if (showProperties)
			for (int i = 0; i < objs.count; i++)
				ImGui::Text(objs.getUiStr(i));
		ImGui::Separator();
		ImGui::Text("Statistics:");
		ImGui::Checkbox("Show", &showStatistics);
		if (showStatistics)
			for (const ObjectStatistics_s& v : data.objectStatistics)
				ImGui::Text("%s | min: %.3f, avg: %.3f, max: %.3f", v.name.c_str(), v.min / kelvinUnitFactor, v.avg / kelvinUnitFactor, v.max / kelvinUnitFactor);		
		ImGui::Separator();
		ImGui::Text("Export:");
		ImGui::SameLine();
		if (ImGui::Button("CSV"))
			export_to_csv_point_values(data.currentValueVector);
		ImGui::SameLine();
		if (ImGui::Button("VTK"))
			export_vtk(data.currentValueVector);
		ImGui::SameLine();
		if (ImGui::Button("Screenshot"))
		{
			int ring_ind = -1;
			for (int i = 0; i < objs.count; i++)
				if (objs.name[i].compare("Ring") == 0)
					ring_ind = i;
			
			std::string path = "./screenshot.png";
			if(ring_ind > -1)
			{
				float dr = objs.diffuseReflectance[ring_ind];
				float sr = objs.specularReflectance[ring_ind];

				std::ostringstream ss;
				ss << "e" << std::setw(4) << std::setfill('0') << std::setprecision(2) << std::fixed << 1 - (dr + sr) << "-d" << dr << "-s" << sr << "-b" << thermalVars.rayDepth << ".png";
				path = ss.str();
			}
			Common::getInstance().takeScreenshot(path, false);
		}
		ImGui::End();
	}
}