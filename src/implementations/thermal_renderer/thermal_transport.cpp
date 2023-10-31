#include "thermal_transport.hpp"

#include <spdlog/stopwatch.h>
#include <GLM\common.hpp>

#include <iostream>
#include<chrono>
#include <sstream>

void ThermalTransport::prepare(rvk::Buffer& geometryDataBuffer, GeometryDataBlasVulkan& _gpuBlas)
{
	// descriptors
	// global descriptor
	globalDescriptor.reserve(8);
	globalDescriptor.addUniformBuffer(GLSL_GLOBAL_UBO_BINDING, rvk::Shader::Stage::RAYGEN);
	globalDescriptor.addStorageBuffer(GLSL_GLOBAL_INSTANCE_DATA_BINDING, rvk::Shader::Stage::RAYGEN | rvk::Shader::Stage::ANY_HIT);
	globalDescriptor.addStorageBuffer(GLSL_GLOBAL_GEOMETRY_DATA_BINDING, rvk::Shader::Stage::RAYGEN | rvk::Shader::Stage::ANY_HIT);
	globalDescriptor.addStorageBuffer(GLSL_GLOBAL_INDEX_BUFFER_BINDING, rvk::Shader::Stage::RAYGEN | rvk::Shader::Stage::ANY_HIT);
	globalDescriptor.addStorageBuffer(GLSL_GLOBAL_VERTEX_BUFFER_BINDING, rvk::Shader::Stage::RAYGEN | rvk::Shader::Stage::ANY_HIT);
	globalDescriptor.addAccelerationStructureKHR(GLSL_GLOBAL_AS_BINDING, rvk::Shader::Stage::RAYGEN);
	globalDescriptor.addStorageBuffer(GLSL_GLOBAL_AREA_DATA_BINDING, rvk::Shader::Stage::RAYGEN);
	globalDescriptor.addStorageBuffer(GLSL_GLOBAL_EMISSION_DATA_BINDING, rvk::Shader::Stage::RAYGEN);
	globalDescriptor.addStorageBuffer(GLSL_GLOBAL_ABSORPTION_DATA_BINDING, rvk::Shader::Stage::RAYGEN);
	globalDescriptor.addStorageBuffer(GLSL_GLOBAL_OUT_IMAGE_BINDING, rvk::Shader::Stage::RAYGEN);
	// set
	globalDescriptor.setBuffer(GLSL_GLOBAL_GEOMETRY_DATA_BINDING, &geometryDataBuffer);
	globalDescriptor.setBuffer(GLSL_GLOBAL_INDEX_BUFFER_BINDING, _gpuBlas.getIndexBuffer());
	globalDescriptor.setBuffer(GLSL_GLOBAL_VERTEX_BUFFER_BINDING, _gpuBlas.getVertexBuffer());
	globalDescriptor.finish(false);

	rt_transport_shader.addStage(rvk::Shader::Source::GLSL, rvk::Shader::Stage::RAYGEN, "./assets/shader/raytracing_thermal/transport_ray.rgen", { "GLSL" });
	rt_transport_shader.addStage(rvk::Shader::Source::GLSL, rvk::Shader::Stage::MISS, "./assets/shader/raytracing_thermal/transport_ray.rmiss", { "GLSL" });
	rt_transport_shader.addStage(rvk::Shader::Source::GLSL, rvk::Shader::Stage::CLOSEST_HIT, "./assets/shader/raytracing_thermal/transport_ray.rchit", { "GLSL" });
	//rt_transport_shader.addStage(rvk::Shader::Source::GLSL, rvk::Shader::Stage::ANY_HIT, "assets/shader/raytracing_thermal/transport_ray.rahit", { "GLSL" });
	rt_transport_shader.addGeneralShaderGroup("./assets/shader/raytracing_thermal/transport_ray.rmiss");
	rt_transport_shader.addGeneralShaderGroup("./assets/shader/raytracing_thermal/transport_ray.rgen");
	rt_transport_shader.addHitShaderGroup("./assets/shader/raytracing_thermal/transport_ray.rchit");// , "assets/shader/raytracing_thermal/transport_ray.rahit");
	rt_transport_shader.finish();

	rt_transport_pipeline.setShader(&rt_transport_shader);
	rt_transport_pipeline.addDescriptorSet({ &globalDescriptor });
	rt_transport_pipeline.finish();
}

void ThermalTransport::initAS(
	rvk::SingleTimeCommand& _stc,
	rvk::LogicalDevice* _device,
	GeometryDataBlasVulkan& _gpuBlas,
	rvk::Buffer& geometryDataBuffer,
	scene_s& _scene,
	ThermalScene& _thermalScene)
{
	top.clear();
	top.reserve(_scene.refModels.size());
	for (RefModel_s* refModel : _scene.refModels) {
		rvk::ASInstance as_instance(_gpuBlas.getBlas(refModel->model));
		glm::mat4 model_matrix = glm::transpose(refModel->model_matrix);
		as_instance.setTransform(&model_matrix[0][0]);
		if (refModel->model->getCustomProperty("traceable").getInt())
			refModel->mask = 0xff;
		else
			refModel->mask = 0x0f;

		as_instance.setMask(refModel->mask);
		top.addInstance(as_instance);
	}

	top.prepare(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR);
	_stc.begin();
	top.CMD_Build(_stc.buffer());
	_stc.end();
	// add the tlas to the descriptor and update it
	globalDescriptor.setAccelerationStructureKHR(GLSL_GLOBAL_AS_BINDING, &top);

	// geometry lookup buffer to find the correct vertex informations in shader during ray tracing
	GeometrySSBO geometry[VK_GEOMETRY_SIZE] = {};

	int geometry_count = 0;
	int vertex_count = 0;
	int triangle_count = 0;

	for (RefModel_s* refModel : _scene.refModels) {
		// each mesh in our model will be a geometry of this models blas
		for (RefMesh_s* refMesh : refModel->refMeshes) {
			Mesh* m = refMesh->mesh;
			GeometryDataVulkan::primitveBufferOffset_s offsets = _gpuBlas.getOffset(m);
			// add geometry infos to our geometry lookup buffer
			geometry[geometry_count].index_buffer_offset = offsets.mIndexOffset;
			geometry[geometry_count].vertex_buffer_offset = offsets.mVertexOffset;
			geometry[geometry_count].has_indices = m->hasIndices();
			std::memcpy(&geometry[geometry_count].baseColorFactor, &refMesh->mesh->getMaterial()->getBaseColorFactor()[0], sizeof(glm::vec4));
			geometry[geometry_count].baseColorTexIdx = -1;
			if (refMesh->mesh->getMaterial()->hasBaseColorTexture()) geometry[geometry_count].baseColorTexIdx = refMesh->mesh->getMaterial()->getBaseColorTexture()->index;
			geometry[geometry_count].alphaCutoff = refMesh->mesh->getMaterial()->getAlphaDiscardValue();
			geometry[geometry_count].triangleCount = m->getVertexCount() / 3;

			// index
			if (m->hasIndices()) {
				triangle_count += m->getIndexCount() / 3;
				geometry[geometry_count].triangleCount = m->getIndexCount() / 3;
			}
			else {
				triangle_count += m->getVertexCount() / 3;
			}
			geometry_count++;

			// vertex
			vertex_count += m->getVertexCount();
		}
	}
	geometryDataBuffer.STC_UploadData(&_stc, &geometry, geometry_count * sizeof(GeometrySSBO));

	// add the tlas to the descriptor and update it
	globalDescriptor.setAccelerationStructureKHR(GLSL_GLOBAL_AS_BINDING, &top);

	_thermalScene.getProperties().vertexCount = vertex_count;
	_thermalScene.getProperties().triangleCount = triangle_count;
}

void ThermalTransport::setupBuffers(unsigned int _vertex_count, unsigned int _triangle_count)
{
	int vertex_matrix_size = glm::max((unsigned int)1, _vertex_count * _vertex_count);

	// create
	instanceDataBuffer.create(rvk::Buffer::Use::STORAGE, VK_INSTANCE_SIZE * sizeof(InstanceSSBO), rvk::Buffer::Location::HOST_COHERENT);
	transportBuffer.create(rvk::Buffer::Use::STORAGE, vertex_matrix_size * sizeof(FLOAT), rvk::Buffer::Location::DEVICE);
	globalUniformBuffer.create(rvk::Buffer::Use::UNIFORM, sizeof(AuxiliaryUbo), rvk::Buffer::Location::DEVICE);
	triangleAreaBuffer.create(rvk::Buffer::Use::STORAGE, _triangle_count * sizeof(FLOAT), rvk::Buffer::Location::DEVICE);
	vertexEmissionBuffer.create(rvk::Buffer::Use::STORAGE, _vertex_count * sizeof(FLOAT), rvk::Buffer::Location::DEVICE);
	vertexAbsorptionBuffer.create(rvk::Buffer::Use::STORAGE, _vertex_count * sizeof(FLOAT), rvk::Buffer::Location::DEVICE);
	valueBuffer.create(rvk::Buffer::Use::STORAGE, _vertex_count * sizeof(FLOAT), rvk::Buffer::Location::DEVICE);

	// map
	instanceDataBuffer.mapBuffer();

	// set
	globalDescriptor.setBuffer(GLSL_GLOBAL_INSTANCE_DATA_BINDING, &instanceDataBuffer);
	globalDescriptor.setBuffer(GLSL_GLOBAL_OUT_IMAGE_BINDING, &transportBuffer);
	globalDescriptor.setBuffer(GLSL_GLOBAL_UBO_BINDING, &globalUniformBuffer);
	globalDescriptor.setBuffer(GLSL_GLOBAL_AREA_DATA_BINDING, &triangleAreaBuffer);
	globalDescriptor.setBuffer(GLSL_GLOBAL_EMISSION_DATA_BINDING, &vertexEmissionBuffer);
	globalDescriptor.setBuffer(GLSL_GLOBAL_ABSORPTION_DATA_BINDING, &vertexAbsorptionBuffer);
}

void ThermalTransport::initTransportBuffer(rvk::SingleTimeCommand& _stc, unsigned int _vertex_count)
{
	transportBuffer.STC_UploadData(&_stc, transportMatrix.data(), transportMatrix.size() * sizeof(FLOAT), 0);
}

void ThermalTransport::setAuxiliaryUbo(AuxiliaryUbo& _aux_ubo, unsigned int _vertex_count, unsigned int _ray_count, unsigned int _rayDepth, unsigned int _batchSeed)
{
	// only set values used for transport
	_aux_ubo.instanceCount = top.size();
	_aux_ubo.vertexCount = _vertex_count;
	_aux_ubo.rayCount = _ray_count;
	_aux_ubo.rayDepth = _rayDepth;
	_aux_ubo.batchSeed = _batchSeed;
}

void ThermalTransport::initAuxilaryBuffer(rvk::SingleTimeCommand& _stc, unsigned int _vertex_count, unsigned int _ray_count, unsigned int _rayDepth, unsigned int _batchSeed)
{
	AuxiliaryUbo aux_ubo;
	setAuxiliaryUbo(aux_ubo, _vertex_count, _ray_count, _rayDepth, _batchSeed);
	globalUniformBuffer.STC_UploadData(&_stc, &aux_ubo, sizeof(AuxiliaryUbo));
}

void ThermalTransport::initInstanceBuffer(rvk::SingleTimeCommand& _stc, scene_s& _scene, ThermalScene& _thermalScene)
{
	int i = 0;
	unsigned int vertex_offset = 0;
	unsigned int triangle_index_offset = 0;
	int geometry_offset = 0;

	InstanceSSBO instance[VK_INSTANCE_SIZE] = {};
	ThermalObjects& _objects = _thermalScene.getObjects();
	for (RefModel_s* refModel : _scene.refModels)
	{
		// glm stores matrices in a column-major order but vulkan acceleration structures requiere a row-major order
		glm::mat4 model_matrix = glm::transpose(refModel->model_matrix);
		// add instance infos for our instance lookup buffer
		instance[i].geometry_buffer_offset = geometry_offset;
		geometry_offset += refModel->refMeshes.size();
		instance[i].geometryCount = refModel->refMeshes.size();
		std::memcpy(&instance[i].model_matrix, &refModel->model_matrix[0], sizeof(glm::mat4));
		glm::mat4 trp_inv_model_matrix = glm::transpose(glm::inverse(refModel->model_matrix));
		std::memcpy(&instance[i].trp_inv_model_matrix, &trp_inv_model_matrix[0], sizeof(glm::mat4));

		instance[i].diffuseReflectance = _objects.diffuseReflectance[i];
		instance[i].specularReflectance = _objects.specularReflectance[i];
		instance[i].absorption = _objects.absorption[i];
		instance[i].diffuseEmission = _objects.diffuseEmission[i];
		instance[i].traceable = _objects.traceable[i];

		i++;
	}

	instanceDataBuffer.STC_UploadData(&_stc, &instance, i * sizeof(InstanceSSBO));
}


void ThermalTransport::initTriangleAreaBuffer(rvk::SingleTimeCommand& _stc, ThermalData& _thermalData, unsigned int triangle_count)
{
	logEigenBase("triangleAreaVector", _thermalData.triangleAreaVector.transpose());

	VectorXf tmp = _thermalData.triangleAreaVector.cast<FLOAT>();
	triangleAreaBuffer.STC_UploadData(&_stc, tmp.data(), tmp.size() * sizeof(FLOAT), 0);
}

void ThermalTransport::initAbsorptionEmissionBuffers(rvk::SingleTimeCommand& _stc, ThermalData& _thermalData, unsigned int vertex_count)
{
	VectorXf tmp;
	tmp = _thermalData.emissionVector.cast<FLOAT>();
	vertexEmissionBuffer.STC_UploadData(&_stc, tmp.data(), tmp.size() * sizeof(FLOAT), 0);
	tmp = _thermalData.absorptionVector.cast<FLOAT>();
	vertexAbsorptionBuffer.STC_UploadData(&_stc, tmp.data(), tmp.size() * sizeof(FLOAT), 0);
}

void ThermalTransport::initKelvinBuffer(rvk::SingleTimeCommand& _stc, const ThermalData& _thermalData, unsigned int vertex_count)
{
	valueBuffer.STC_UploadData(&_stc, _thermalData.initialValueVector.data(), _thermalData.initialValueVector.size() * sizeof(FLOAT), 0);
}

void ThermalTransport::load(
	rvk::SingleTimeCommand& _stc,
	rvk::LogicalDevice* _device,
	GeometryDataBlasVulkan& _gpuBlas,
	rvk::Buffer& geometryDataBuffer,
	scene_s& _scene,
	ThermalScene& _thermalScene,
	ThermalData& _thermalData,
	unsigned int _batch_count,
	unsigned int _ray_count,
	unsigned int _ray_depth,
	int mode,
	bool setup)
{
	CHECK_EMPTY_SCENE(_scene)

	unsigned int vertex_count = _thermalScene.getProperties().vertexCount;
	unsigned int triangle_count = _thermalScene.getProperties().triangleCount;

	int n = glm::max((unsigned int)1, vertex_count);
	transportMatrix = Mat::Constant(n, n, 0.0);
	spdlog::info("ThermalRenderer: created transport matrix of size {} x {}", n, n);

	if(setup)
		setupBuffers(vertex_count, triangle_count);

	initAS(_stc, _device, _gpuBlas, geometryDataBuffer, _scene, _thermalScene);
	initTransportBuffer(_stc, vertex_count);
	initAuxilaryBuffer(_stc, vertex_count, _ray_count, 0, 0);
	initInstanceBuffer(_stc, _scene, _thermalScene);
	initTriangleAreaBuffer(_stc, _thermalData, triangle_count);
	initAbsorptionEmissionBuffers(_stc, _thermalData, vertex_count);
	initKelvinBuffer(_stc, _thermalData, vertex_count);

	globalDescriptor.update();
}

void ThermalTransport::compute(rvk::SingleTimeCommand& stc, rvk::LogicalDevice* _device, ThermalScene& _thermalScene, ThermalData& _thermalData, unsigned int _batch_count, unsigned int _ray_count, unsigned int _ray_depth, int mode)
{
	spdlog::info("computeTransportMatrix: Spawning threads for {} triangles...", _thermalScene.getProperties().triangleCount);

	spdlog::stopwatch sw_gpu;

	int n = _thermalScene.getProperties().vertexCount;

	for (int i = 0; i < _batch_count; i++)
	{
		spdlog::info("\tBatch: {}/{}, ray count: {}, ray depth: {} ...", i + 1, _batch_count, _ray_count, _ray_depth);

		AuxiliaryUbo aux_ubo;
		setAuxiliaryUbo(aux_ubo, n, _ray_count, _ray_depth, i);
		globalUniformBuffer.STC_UploadData(&stc, &aux_ubo, sizeof(AuxiliaryUbo));
		globalDescriptor.update();

		stc.begin();
		rt_transport_pipeline.CMD_BindDescriptorSets(stc.buffer(), { &globalDescriptor });
		rt_transport_pipeline.CMD_BindPipeline(stc.buffer());
		rt_transport_pipeline.CMD_TraceRays(stc.buffer(), 1, _thermalScene.getProperties().triangleCount);
		stc.end();
	}
	_device->waitIdle();

	auto gpu_time = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(sw_gpu.elapsed()).count() / 1000.0);
	spdlog::info("\tfinished. (dur.: {:.3} s)", gpu_time);

	spdlog::stopwatch sw_cpu;

	transportBuffer.STC_DownloadData(&stc, transportMatrix.data(), transportMatrix.size() * sizeof(FLOAT));

	logEigenBase("solverData.transportMatrix", transportMatrix);

	if (transportMatrix.hasNaN())
		spdlog::error("Transport matrix has NaN!");

	unsigned int emit_count = _ray_count * _batch_count;
	for (int k = 0; k < n; k++)
		transportMatrix.col(k) /= 3.0 * emit_count * _thermalData.vertexTriangleCountVector(k);

	logEigenBase("solverData.transportMatrix", transportMatrix);

#ifndef RUNTIME_OPTIMIZED
	spdlog::debug("HINT: rows represent the 'sum' of emitted and absorbed factor for each node, if sum < 0 node is potentially 'loosing' energy, if sum > 0 potentially 'gaining' energy");
	spdlog::debug("HINT: columns represent the 'sum' of emitted and distributed factor per node, if sum < 0 some energy is not absorbed, if sum > 0 something is wrong");
	spdlog::debug("HINT: for closed systems the column sum must be 0");
	printTransportMatrixSums();

	spdlog::info("transportMatrix:min = {:.2}", transportMatrix.minCoeff());
	spdlog::info("transportMatrix:avg = {:.2}", transportMatrix.mean());
	spdlog::info("transportMatrix:max = {:.2}", transportMatrix.maxCoeff());
#endif // RUNTIME_OPTIMIZED

	if (mode == 0)
	{
		// scale rows by emission 
		for (int k = 0; k < n; k++)
			transportMatrix.row(k).array() *= _thermalData.emissionVector.array();

		logEigenBase("thermalVars.emissionVector", _thermalData.emissionVector.transpose());
		logEigenBase("solverData.transportMatrix", transportMatrix);

		// scale columns by absorption 
		for (int k = 0; k < n; k++)
			transportMatrix.col(k).array() *= _thermalData.absorptionVector.array();

		logEigenBase("thermalVars.absorptionVector", _thermalData.absorptionVector.transpose());
		logEigenBase("solverData.transportMatrix", transportMatrix);

		transportMatrix *= STEFAN_BOLTZMANN_CONST;
	}
	else
	{
		for (int k = 0; k < n; k++)
			transportMatrix.row(k).array() *= _thermalData.vertexAreaVector.array();

		for (int k = 0; k < n; k++)
			transportMatrix.col(k).array() *= 1.0 / _thermalData.vertexAreaVector.array();
	}

	spdlog::info("Transport matrix generation dur.: {:.3} s; (GPU: {:.3} s, CPU: {:.3} s,))", sw_gpu, gpu_time, sw_cpu);
	//spdlog::info("Transport matrix condition number ...");
	//spdlog::info("Transport matrix condition number: {:.3}", condistion_number(transportMatrix));

#ifndef RUNTIME_OPTIMIZED
	spdlog::info("transportMatrix:min = {:.2}", transportMatrix.minCoeff());
	spdlog::info("transportMatrix:avg = {:.2}", transportMatrix.mean());
	spdlog::info("transportMatrix:max = {:.2}", transportMatrix.maxCoeff());
#endif

#ifndef DISABLE_GUI
	uploadTransportMatrix(stc, transportMatrix);
#endif

#ifndef RUNTIME_OPTIMIZED
	printTransportMatrixSums();
#endif // RUNTIME_OPTIMIZED
}

void ThermalTransport::uploadTransportMatrix(rvk::SingleTimeCommand& stc, const Mat& _transportMatrix)
{
	transportBuffer.STC_UploadData(&stc, _transportMatrix.data(), _transportMatrix.size() * sizeof(FLOAT), 0);
}

void ThermalTransport::uploadValueVector(rvk::SingleTimeCommand& stc, const Vec& _values)
{
	valueBuffer.STC_UploadData(&stc, _values.data(), _values.size() * sizeof(FLOAT), 0);
}

void ThermalTransport::unload()
{
	top.destroy();
	transportBuffer.destroy();
	valueBuffer.destroy();
	triangleAreaBuffer.destroy();
	vertexEmissionBuffer.destroy();
	vertexAbsorptionBuffer.destroy();
}

void ThermalTransport::printTransportMatrixSums()
{
	//return;
	SCALAR threshold;
	threshold = FLT_EPSILON;
	if (spdlog::get_level() >= spdlog::level::debug)
	{
		for (int k = 0; k < transportMatrix.cols(); k++) {
			//if (std::abs(transportMatrix.col(k).sum()) > (10.0 * threshold))
			spdlog::debug("transport column {} sum {}", k, transportMatrix.col(k).sum());
		}
		for (int k = 0; k < transportMatrix.rows(); k++) {
			//if (std::abs(transportMatrix.row(k).sum()) > (10.0 * threshold))
			spdlog::debug("transport   row  {} sum {}", k, transportMatrix.row(k).sum());
		}
	}
}