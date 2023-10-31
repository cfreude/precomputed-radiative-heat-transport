#include "thermal_export.hpp"

#include <tamashii/engine/common/common.hpp>
#include <tamashii/engine/render/render_backend_implementation.hpp>
#include <tamashii/renderer_vk/render_backend.hpp>
#include <rvk/rvk.hpp>

#include <sstream>
#include <iostream>
#include <fstream>

RVK_USE_NAMESPACE
using namespace tamashii;
using namespace Eigen;

void export_to_csv_point_values(const Vec& _values)
{
	std::ofstream myfile;
	myfile.open("data.csv");
	myfile << "\"temperature\", \"Points:0\", \"Points:1\", \"Points:2\"\n";
	Eigen::VectorXf tmp = _values.cast<FLOAT>();

	spdlog::info("tmp count:{}, size:{}", tmp.count(), tmp.size());
	for (int i = 0; i < tmp.count(); i++)
	{
		float val = tmp[i] / kelvinUnitFactor;
		myfile << std::fixed << std::setprecision(5) << val << "\n";
		spdlog::info("val[{}] = {:.3}", i, val);
	}
	myfile.close();
}

void export_to_csv_object_avg(const std::vector<ObjectStatistics_s>& _statistics)
{
	std::ofstream myfile;
	myfile.open("obj-avgs.csv");
	for (const ObjectStatistics_s& v : _statistics)
		myfile << v.name << "; " << std::fixed << std::setprecision(5) << v.avg / kelvinUnitFactor << "\n";
	myfile.close();
}

void export_vtk(const Vec& _values)
{
	RenderScene* _scene = Common::getInstance().getRenderSystem()->getMainScene();
	scene_s scene = _scene->getSceneData();

	int vertex_count = 0;
	int triangle_count = 0;

	for (RefModel_s* refModel : scene.refModels) {
		for (RefMesh_s* refMesh : refModel->refMeshes) {
			Mesh* m = refMesh->mesh;
			vertex_count += m->getVertexCount();
			triangle_count += m->getPrimitiveCount();
		}
	}

	int nNodes = vertex_count;
	int nCoordsPerNode = 3;
	int nElems = triangle_count;
	int nNodesPerElem = 3;
	int cellType = 5;

	std::ofstream out("./scene.vtk");

	if (!out.is_open())
	{
		spdlog::error("Can not open file for VTK export.");
	}
	else
	{
		out.precision(12);

		//write header
		out << "# vtk DataFile Version 2.0" << std::endl;
		out << "VTK exported mesh" << std::endl;
		out << "ASCII" << std::endl;
		out << "DATASET UNSTRUCTURED_GRID" << std::endl;

		//node coordinates
		out << "POINTS " << nNodes << " double" << std::endl;

		for (RefModel_s* refModel : scene.refModels) {
			glm::mat4 model_matrix = glm::transpose(refModel->model_matrix);
			for (RefMesh_s* refMesh : refModel->refMeshes) {
				Mesh* m = refMesh->mesh;
				for (const auto& v : *m->getVerticesVector())
				{
					glm::mat4 mat = glm::mat4();
					mat = glm::rotate(mat, glm::half_pi<float>(), glm::vec3(0, 0, 1));
					mat = glm::rotate(refModel->model_matrix, glm::half_pi<float>(), glm::vec3(1, 0, 0));
					glm::vec4 _v = mat * v.position;
					glm::vec3 __v = glm::vec3(_v.x, _v.y, _v.z) / _v.w;
					out << __v.x << " " << __v.y << " " << __v.z << std::endl; // switch Y and Z
				}
			}
		}

		//cells
		int vertex_offset = 0;
		out << "CELLS " << nElems << " " << (1 + nNodesPerElem) * nElems << std::endl;
		for (RefModel_s* refModel : scene.refModels) {
			for (RefMesh_s* refMesh : refModel->refMeshes) {
				Mesh* m = refMesh->mesh;
				int ic = 0;
				auto indices = *m->getIndicesVector();
				for (int i = 0; i < m->getPrimitiveCount(); i++)
					out << "3 " << vertex_offset + indices[ic++] << " " << vertex_offset + indices[ic++] << " " << vertex_offset + indices[ic++] << std::endl;
				vertex_offset += m->getVertexCount();
			}
		}

		//cell types
		out << "CELL_TYPES " << nElems << std::endl;
		for (int i = 0; i < nElems; ++i) out << cellType << std::endl;
		// mesh information is done now, next up point and cell data ...

		out << "POINT_DATA " << nNodes << std::endl;

		out << "SCALARS kelvin double 1" << std::endl;
		out << "LOOKUP_TABLE default" << std::endl;
		vertex_offset = 0;
		for (RefModel_s* refModel : scene.refModels) {
			for (RefMesh_s* refMesh : refModel->refMeshes) {
				Mesh* m = refMesh->mesh;
				for (int i = vertex_offset; i < (vertex_offset + m->getVertexCount()); i++)
					out << _values[i] / kelvinUnitFactor << std::endl;
				vertex_offset += m->getVertexCount();
			}
		}

		if (false)
		{
			out << "NORMALS point_normals double" << std::endl;
			for (RefModel_s* refModel : scene.refModels) {
				for (RefMesh_s* refMesh : refModel->refMeshes) {
					Mesh* m = refMesh->mesh;
					for (const auto& v : *m->getVerticesVector())
						out << v.normal.x << " " << v.normal.y << " " << v.normal.z << std::endl;
				}
			}

		}

		/*out << "CELL_DATA " << nElems << endl;
		for (int i = 0; i < vtkDataNames.size(); ++i) {
			if (vtkDataIsCellData[i] == true) // this is cell data
				vtkWriteData(out, i, nElems);
		}*/
	}
}