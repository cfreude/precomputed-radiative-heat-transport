#include "thermal_data.hpp"

void ThermalData::init(scene_s _scene, const ThermalObjects& _thermal_object, unsigned int _vertex_count, unsigned int _triangle_count)
{
	currentValueVector = Vec(_vertex_count);
	currentValueVector.setConstant(0.0);
	initialValueVector = Vec(_vertex_count);
	initialValueVector.setConstant(0.0);
	emissionVector = Vec(_vertex_count);
	emissionVector.setConstant(0.0);
	absorptionVector = Vec(_vertex_count);
	absorptionVector.setConstant(0.0);

	vertexAreaVector = Vec(_vertex_count);
	vertexAreaVector.setConstant(0.0);

	vertexTriangleCountVector = VectorXi(_vertex_count);
	vertexTriangleCountVector.setConstant(0);

	triangleAreaVector = Vec(_triangle_count);
	triangleAreaVector.setConstant(0.0);

	fixedVarsVector = Vec(_vertex_count);
	fixedVarsVector.setConstant(0.0);
}

void ThermalData::load(scene_s _scene, ThermalObjects& _thermal_objects, SCALAR _sky_minimum_kelvin)
{
	int i = 0;
	unsigned int vertex_offset = 0;
	unsigned int triangle_index_offset = 0;
	int geometry_offset = 0;

	for (RefModel_s* refModel : _scene.refModels) {
		// glm stores matrices in a column-major order but vulkan acceleration structures requiere a row-major order
		glm::mat4 model_matrix = glm::transpose(refModel->model_matrix);
		// add instance infos for our instance lookup buffer

		for (RefMesh_s* refMesh : refModel->refMeshes) {
			Mesh* m = refMesh->mesh;
			for (auto i : *m->getIndicesVector())
				vertexTriangleCountVector(vertex_offset + i) += 1;

			for (int vi = 0; vi < m->getIndexCount(); vi += 3)
			{
				unsigned int ind0 = m->getIndicesVector()->at(vi);
				unsigned int ind1 = m->getIndicesVector()->at(vi + 1);
				unsigned int ind2 = m->getIndicesVector()->at(vi + 2);
				glm::dvec4 v0_ = refModel->model_matrix * m->getVerticesVector()->at(ind0).position;
				glm::dvec4 v1_ = refModel->model_matrix * m->getVerticesVector()->at(ind1).position;
				glm::dvec4 v2_ = refModel->model_matrix * m->getVerticesVector()->at(ind2).position;
				glm::dvec3 v0 = glm::dvec3(v0_.x, v0_.y, v0_.z) / v0_.w;
				glm::dvec3 v1 = glm::dvec3(v1_.x, v1_.y, v1_.z) / v1_.w;
				glm::dvec3 v2 = glm::dvec3(v2_.x, v2_.y, v2_.z) / v2_.w;
				SCALAR area = glm::length(glm::cross<double>(v1 - v0, v2 - v0)) * 0.5;
				if (area < (10.0 * FLT_EPSILON)) {
					//ToDo: what is the right way to deal with small elements?
					//Problem: if a small element (representing a tiny bit of material) randomly gets hit by a ray, it absorbs some amount of energy that leads to a huge change in temperature (because the heat capacity is so small).
					//Result: this behaviour leads to ill-conditioned systems which can limit the time step size quite badly.
					//Also: these tiny elements may not generate many outgoing rays (or those rays that do get generated might hit nearby elements depending on the geometry around them) and may fail to radiate off enough energy to cool down properly.
					//The best way to deal with this would be to make sure we only get "clean" geometry as input.
					spdlog::warn("clamping small element {} area {} --> {}", triangle_index_offset, area, (10.0 * FLT_EPSILON));
					area = (10.0 * FLT_EPSILON);
				}
				vertexAreaVector[vertex_offset + ind0] += area / 3.0;
				vertexAreaVector[vertex_offset + ind1] += area / 3.0;
				vertexAreaVector[vertex_offset + ind2] += area / 3.0;
				triangleAreaVector[triangle_index_offset++] = area;
			}
			fixedVarsVector.segment(vertex_offset, m->getVertexCount()).setConstant(_thermal_objects.temperatureFixed[i]);
			initialValueVector.segment(vertex_offset, m->getVertexCount()).setConstant(_thermal_objects.kelvin[i]);

			// overrride constant initialKelvin
			std::vector<Value> sky_vals = refModel->model->getCustomProperty("sky-values").getArray();
			if (sky_vals.size() > 0)
			{
				SCALAR avg_sky_value = 0.0;

				std::vector<uint32_t> vis = *m->getIndicesVector();
				unsigned int index_offset = 0;
				for (Value val : sky_vals)
				{
					float sky_value = val.getFloat(); // sky value already converted from kWh/m2 to W / m^2
					avg_sky_value += sky_value;

					{
						unsigned int vertex_ind_0 = vertex_offset + vis[index_offset++];
						initialValueVector(vertex_ind_0) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_0], _sky_minimum_kelvin);

						unsigned int vertex_ind_1 = vertex_offset + vis[index_offset++];
						initialValueVector(vertex_ind_1) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_1], _sky_minimum_kelvin);

						unsigned int vertex_ind_2 = vertex_offset + vis[index_offset++];
						initialValueVector(vertex_ind_2) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_2], _sky_minimum_kelvin);
					}

					{
						unsigned int vertex_ind_0 = vertex_offset + vis[index_offset++];
						initialValueVector(vertex_ind_0) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_0], _sky_minimum_kelvin);

						unsigned int vertex_ind_1 = vertex_offset + vis[index_offset++];
						initialValueVector(vertex_ind_1) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_1], _sky_minimum_kelvin);

						unsigned int vertex_ind_2 = vertex_offset + vis[index_offset++];
						initialValueVector(vertex_ind_2) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_2], _sky_minimum_kelvin);
					}
				}

				avg_sky_value /= ((float)sky_vals.size());

				// WARNING: modification of 
				_thermal_objects.kelvin[i] = sky_value_to_kelvin(avg_sky_value, triangleAreaVector.sum(), _sky_minimum_kelvin);
			}

			emissionVector.segment(vertex_offset, m->getVertexCount()).setConstant(_thermal_objects.absorption[i]);//* STEFAN_BOLTZMANN_CONST);
			absorptionVector.segment(vertex_offset, m->getVertexCount()).setConstant(1.0 / (_thermal_objects.density[i] * _thermal_objects.heatCapacity[i] * _thermal_objects.thickness[i]));
			vertex_offset += m->getVertexCount();
		}

		ObjectStatistics_s stats;
		stats.name = std::string(_thermal_objects.name[i]);
		stats.min = _thermal_objects.kelvin[i];
		stats.max = _thermal_objects.kelvin[i];
		stats.avg = _thermal_objects.kelvin[i];
		objectStatistics.push_back(stats);

		i++;
	}

	emissionVector.array() *= vertexAreaVector.array();
	absorptionVector.array() *= 1.0 / vertexAreaVector.array();

	logEigenBase("vertexAreaVector", vertexAreaVector.transpose());
	logEigenBase("vertexTriangleCount", vertexTriangleCountVector.transpose());
}

void ThermalData::setObjectStatistics(const ThermalObjects& objects, const Vec& _values)
{
	unsigned int i = 0;
	for (ObjectStatistics_s& v : objectStatistics)
	{
		if (_values.count() > 0) {
			Vec vals = _values.segment(objects.vertexOffset[i], objects.vertexCount[i]);
			v.min = vals.minCoeff();
			v.max = vals.maxCoeff();
			v.avg = vals.mean();
		}
		i++;
	}
}

void ThermalData::reset()
{
	currentValueVector = initialValueVector;
}

void ThermalData::unload()
{
	currentValueVector.resize(0);
	initialValueVector.resize(0);
	emissionVector.resize(0);
	absorptionVector.resize(0);
	vertexAreaVector.resize(0);
	vertexTriangleCountVector.resize(0);
	triangleAreaVector.resize(0);
	fixedVarsVector.resize(0);
	objectStatistics.clear();
}
