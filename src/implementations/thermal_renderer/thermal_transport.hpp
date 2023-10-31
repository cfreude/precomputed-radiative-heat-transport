#pragma once
#include <glm/glm.hpp>

#include <rvk/rvk.hpp>
#include <tamashii/engine/common/vars.hpp>
#include <tamashii/engine/scene/render_scene.hpp>
#include <tamashii/renderer_vk/convenience/geometry_to_gpu.hpp>
#include <tamashii/engine/render/render_backend_implementation.hpp>

#include "../../../assets/shader/raytracing_thermal/defines.h"

#include "thermal_common.hpp"
#include "thermal_objects.hpp"
#include "thermal_data.hpp"
#include "thermal_scene.hpp"
#include "thermal_objects.hpp"

T_USE_NAMESPACE

using namespace Eigen;

class ThermalTransport {

public:

	ThermalTransport(rvk::LogicalDevice* aDevice) :
		top(aDevice),
		instanceDataBuffer(aDevice),
		globalDescriptor(aDevice),
		globalUniformBuffer(aDevice),
		rtImage(aDevice),
		rt_transport_shader(aDevice),
		rt_transport_pipeline(aDevice),
		transportBuffer(aDevice),
		triangleAreaBuffer(aDevice),
		valueBuffer(aDevice),
		vertexEmissionBuffer(aDevice),
		vertexAbsorptionBuffer(aDevice)
	{}

	~ThermalTransport() = default;

	void prepare(rvk::Buffer& _geomBuffer, GeometryDataBlasVulkan& _gpuBlas);
	void load(rvk::SingleTimeCommand& _stc, rvk::LogicalDevice* _device, GeometryDataBlasVulkan& _gpuBlas, rvk::Buffer& geometryDataBuffer, scene_s& _scene, ThermalScene& _thermalScene, ThermalData& _thermalData, unsigned int _batch_count, unsigned int _ray_count, unsigned int _ray_depth, int mode, bool _setup = false);
	void initAS(rvk::SingleTimeCommand& _stc, rvk::LogicalDevice* _device, GeometryDataBlasVulkan& _gpuBlas, rvk::Buffer& geometryDataBuffer, scene_s& _scene, ThermalScene& _thermalScene);
	void setupBuffers(unsigned int _vertex_count, unsigned int _triangle_count);
	void compute(rvk::SingleTimeCommand& stc, rvk::LogicalDevice* _device, ThermalScene& _thermalScene, ThermalData& _thermalData, unsigned int batchCount, unsigned int rayCount, unsigned int _ray_depth, int mode);
	//void recompute(viewDef_s* aViewDef, rvk::SingleTimeCommand& stc, rvk::LogicalDevice* _device, GeometryDataBlasVulkan& _gpuBlas, ThermalScene& _thermalScene, ThermalData& _thermalData, unsigned int batchCount, unsigned int rayCount, int mode);

	void initTransportBuffer(rvk::SingleTimeCommand& _stc, unsigned int _vertex_count);
	void initAuxilaryBuffer(rvk::SingleTimeCommand& _stc, unsigned int _vertex_count, unsigned int _ray_count, unsigned int _rayDepth, unsigned int _batchSeed);
	void initInstanceBuffer(rvk::SingleTimeCommand& _stc, scene_s& _scene, ThermalScene& _thermalScene);
	void initTriangleAreaBuffer(rvk::SingleTimeCommand& _stc, ThermalData& _thermalData, unsigned int traingle_count);
	void initAbsorptionEmissionBuffers(rvk::SingleTimeCommand& _stc, ThermalData& _thermalData, unsigned int vertex_count);
	void initKelvinBuffer(rvk::SingleTimeCommand& _stc, const ThermalData& _thermalData, unsigned int vertex_count);

	void setAuxiliaryUbo(AuxiliaryUbo& _aux_ubo, unsigned int _vertex_count, unsigned int _ray_count, unsigned int _rayDepth, unsigned int _batchSeed);

	void uploadTransportMatrix(rvk::SingleTimeCommand& stc, const Mat& _transportMatrix);
	void uploadValueVector(rvk::SingleTimeCommand& stc, const Vec& _values);

	void unload();

	void printTransportMatrixSums();

	const Mat& getTransportMatrix() { return transportMatrix; }

	rvk::Buffer& getTransportBuffer() { return transportBuffer; }
	rvk::Buffer& getKelvinBuffer() { return valueBuffer; }

private:

	#define VK_INDEX_DATA_SIZE 1024 * 1024 * 4
	#define VK_VERTEX_DATA_SIZE 512 * 1024 * 4
	#define VK_GLOBAL_IMAGE_SIZE 128
	#define VK_INSTANCE_SIZE 256 * 4
	#define VK_GEOMETRY_SIZE 256 * 4

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	// tlas
	rvk::TopLevelAS										top;

	rvk::Buffer											instanceDataBuffer;
	rvk::Descriptor										globalDescriptor;
	// buffers
	rvk::Buffer											globalUniformBuffer;
	rvk::Image											rtImage;

	rvk::RTShader										rt_transport_shader;
	rvk::RTPipeline										rt_transport_pipeline;

	rvk::Buffer											transportBuffer;
	rvk::Buffer											triangleAreaBuffer;

	rvk::Buffer											valueBuffer;
	rvk::Buffer											vertexEmissionBuffer;
	rvk::Buffer											vertexAbsorptionBuffer;

	Mat transportMatrix;

};