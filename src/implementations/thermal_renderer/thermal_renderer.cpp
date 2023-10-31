#include "thermal_renderer.hpp"

#include <imgui.h>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <limits>
#include <math.h>
#include <map>
#include <fstream>

#include <spdlog/stopwatch.h>

/* engine */
#include <tamashii/engine/common/common.hpp>
#include <tamashii/engine/common/vars.hpp>
#include <tamashii/engine/platform/filewatcher.hpp>
#include <thermal_common.hpp>

RVK_USE_NAMESPACE
using namespace tamashii;

#define VK_INDEX_DATA_SIZE 1024 * 1024 * 4
#define VK_VERTEX_DATA_SIZE 512 * 1024 * 4
#define VK_GLOBAL_IMAGE_SIZE 128
#define VK_INSTANCE_SIZE 256 * 4
#define VK_GEOMETRY_SIZE 256 * 4

/*
TODOs:
- FIX solver NaN
- solve FEM math
- think about switching to triangle based matrix?
- check if there is bug in raygen shader ThreadID > 1000
- add conduction
- FIX transport recomputationlogic/button
- make object properties editable
*/

void ThermalRenderer::windowSizeChanged(const int aWidth, const int aHeight) {
	SingleTimeCommand stc = mGetStcBuffer();
	stc.begin();
	for (uint32_t si_idx = 0; si_idx < mFrameCount; si_idx++) {
		VkFrameData& frameData = mVkFrameData[si_idx];

		frameData.rtImage.destroy();
		// rtImage
		frameData.rtImage.createImage2D(aWidth, aHeight, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
		frameData.rtImage.CMD_TransitionImage(stc.buffer(), VK_IMAGE_LAYOUT_GENERAL);
		frameData.globalDescriptor.setImage(GLSL_GLOBAL_OUT_IMAGE_BINDING, &frameData.rtImage);
		if (frameData.top.size()) frameData.globalDescriptor.update();
	}
	stc.end();
}

// implementation preparation
void ThermalRenderer::prepare(renderInfo_s* aRenderInfo) {

	//spdlog::set_level(spdlog::level::level_enum::info);
	//spdlog::set_level(spdlog::level::level_enum::debug);
	//spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

	printVramSize(mDevice);

	prepareData(aRenderInfo);

	mThermalTransport.prepare(mVkData->geometryDataBuffer, this->blas_gpu);

	createSunAndSky = false;
	sunDirections.push_back(Vector3f(1, 1, 1));
}

void ThermalRenderer::prepareData(renderInfo_s* aRenderInfo)
{
	printVramSize(mDevice);

	mVkData.emplace(mDevice);

	blas_gpu.prepare(VK_INDEX_DATA_SIZE, VK_VERTEX_DATA_SIZE);

	// geometry
	mVkData->geometryDataBuffer.create(rvk::Buffer::Use::STORAGE, VK_GEOMETRY_SIZE * sizeof(GeometrySSBO), rvk::Buffer::Location::HOST_COHERENT);
	mVkData->geometryDataBuffer.mapBuffer();

#ifndef DISABLE_GUI

	mVkFrameData.resize(mFrameCount, VkFrameData(mDevice));
	td_gpu.prepare(rvk::Shader::Stage::RAYGEN | rvk::Shader::Stage::ANY_HIT, VK_GLOBAL_IMAGE_SIZE);

	// lights
	mVkData->pointLightBuffer.create(rvk::Buffer::Use::STORAGE, sizeof(LightsSSBO), rvk::Buffer::Location::HOST_COHERENT);
	mVkData->pointLightBuffer.mapBuffer();

	SingleTimeCommand stc = mGetStcBuffer();
	stc.begin();
	// data for individual frames
	for (uint32_t si_idx = 0; si_idx < mFrameCount; si_idx++) {
		VkFrameData& frameData = mVkFrameData[si_idx];

		// instance
		frameData.instanceDataBuffer.create(rvk::Buffer::Use::STORAGE, VK_INSTANCE_SIZE * sizeof(InstanceSSBO), rvk::Buffer::Location::HOST_COHERENT);
		frameData.instanceDataBuffer.mapBuffer();
		// rtImage
		frameData.rtImage.createImage2D(aRenderInfo->target_size.x, aRenderInfo->target_size.y, VK_FORMAT_B8G8R8A8_UNORM, rvk::Image::Use::DOWNLOAD | rvk::Image::Use::UPLOAD | rvk::Image::Use::STORAGE);
		frameData.rtImage.CMD_TransitionImage(stc.buffer(), VK_IMAGE_LAYOUT_GENERAL);

		// global uniform buffer
		frameData.globalUniformBuffer.create(rvk::Buffer::Use::UNIFORM, sizeof(GlobalUbo), rvk::Buffer::Location::HOST_COHERENT);
		frameData.globalUniformBuffer.mapBuffer();

		// global uniform aux buffer
		frameData.globalAuxUniformBuffer.create(rvk::Buffer::Use::UNIFORM, sizeof(AuxiliaryUbo), rvk::Buffer::Location::HOST_COHERENT);
		frameData.globalAuxUniformBuffer.mapBuffer();

		// descriptors
		// global descriptor
		frameData.globalDescriptor.reserve(10);
		frameData.globalDescriptor.addUniformBuffer(GLSL_GLOBAL_UBO_BINDING, rvk::Shader::Stage::RAYGEN);
		frameData.globalDescriptor.addUniformBuffer(GLSL_GLOBAL_AUX_UBO_BINDING, rvk::Shader::Stage::RAYGEN);
		frameData.globalDescriptor.addStorageBuffer(GLSL_GLOBAL_INSTANCE_DATA_BINDING, rvk::Shader::Stage::RAYGEN | rvk::Shader::Stage::ANY_HIT);
		frameData.globalDescriptor.addStorageBuffer(GLSL_GLOBAL_GEOMETRY_DATA_BINDING, rvk::Shader::Stage::RAYGEN | rvk::Shader::Stage::ANY_HIT);
		frameData.globalDescriptor.addStorageBuffer(GLSL_GLOBAL_INDEX_BUFFER_BINDING, rvk::Shader::Stage::RAYGEN | rvk::Shader::Stage::ANY_HIT);
		frameData.globalDescriptor.addStorageBuffer(GLSL_GLOBAL_VERTEX_BUFFER_BINDING, rvk::Shader::Stage::RAYGEN | rvk::Shader::Stage::ANY_HIT);
		frameData.globalDescriptor.addStorageBuffer(GLSL_GLOBAL_TRANSPORT_DATA_BINDING, rvk::Shader::Stage::RAYGEN);
		frameData.globalDescriptor.addStorageBuffer(GLSL_GLOBAL_KELVIN_DATA_BINDING, rvk::Shader::Stage::RAYGEN);
		frameData.globalDescriptor.addStorageBuffer(GLSL_GLOBAL_LIGHT_DATA_BINDING, rvk::Shader::Stage::RAYGEN);
		frameData.globalDescriptor.addAccelerationStructureKHR(GLSL_GLOBAL_AS_BINDING, rvk::Shader::Stage::RAYGEN);
		frameData.globalDescriptor.addStorageImage(GLSL_GLOBAL_OUT_IMAGE_BINDING, rvk::Shader::Stage::RAYGEN);
		// set
		frameData.globalDescriptor.setBuffer(GLSL_GLOBAL_UBO_BINDING, &frameData.globalUniformBuffer);
		frameData.globalDescriptor.setBuffer(GLSL_GLOBAL_AUX_UBO_BINDING, &frameData.globalAuxUniformBuffer);
		frameData.globalDescriptor.setBuffer(GLSL_GLOBAL_INSTANCE_DATA_BINDING, &frameData.instanceDataBuffer);
		frameData.globalDescriptor.setBuffer(GLSL_GLOBAL_GEOMETRY_DATA_BINDING, &mVkData->geometryDataBuffer);
		frameData.globalDescriptor.setBuffer(GLSL_GLOBAL_INDEX_BUFFER_BINDING, this->blas_gpu.getIndexBuffer());
		frameData.globalDescriptor.setBuffer(GLSL_GLOBAL_VERTEX_BUFFER_BINDING, this->blas_gpu.getVertexBuffer());
		frameData.globalDescriptor.setBuffer(GLSL_GLOBAL_LIGHT_DATA_BINDING, &mVkData->pointLightBuffer);
		frameData.globalDescriptor.setImage(GLSL_GLOBAL_OUT_IMAGE_BINDING, &frameData.rtImage);
		frameData.globalDescriptor.finish(false);
	}
	stc.end();

	mVkData->rtshader.addStage(rvk::Shader::Source::GLSL, rvk::Shader::Stage::RAYGEN, "./assets/shader/raytracing_thermal/simple_ray.rgen", { "GLSL" });
	mVkData->rtshader.addStage(rvk::Shader::Source::GLSL, rvk::Shader::Stage::MISS, "./assets/shader/raytracing_thermal/simple_ray.rmiss", { "GLSL" });
	mVkData->rtshader.addStage(rvk::Shader::Source::GLSL, rvk::Shader::Stage::CLOSEST_HIT, "./assets/shader/raytracing_thermal/simple_ray.rchit", { "GLSL" });
	mVkData->rtshader.addStage(rvk::Shader::Source::GLSL, rvk::Shader::Stage::ANY_HIT, "./assets/shader/raytracing_thermal/simple_ray.rahit", { "GLSL" });
	mVkData->rtshader.addGeneralShaderGroup("./assets/shader/raytracing_thermal/simple_ray.rmiss");
	mVkData->rtshader.addGeneralShaderGroup("./assets/shader/raytracing_thermal/simple_ray.rgen");
	mVkData->rtshader.addHitShaderGroup("./assets/shader/raytracing_thermal/simple_ray.rchit", "./assets/shader/raytracing_thermal/simple_ray.rahit");
	mVkData->rtshader.finish();
	FileWatcher::getInstance().watchFile("./assets/shader/raytracing_thermal/simple_ray.rgen", [this]() { mVkData->rtshader.reloadShader("./assets/shader/raytracing_thermal/simple_ray.rgen"); });

	mVkData->rtpipeline.setShader(&mVkData->rtshader);
	mVkData->rtpipeline.addDescriptorSet({ &mVkFrameData[0].globalDescriptor, td_gpu.getDescriptor() });
	mVkData->rtpipeline.finish();

#endif // DISABLE_GUI
}

void ThermalRenderer::destroy() {
	FileWatcher::getInstance().removeFile("assets/shader/raytracing_thermal/simple_ray.rgen");

	td_gpu.destroy();
	blas_gpu.destroy();
	mVkFrameData.clear();
	mVkData.reset();
	mThermalTransport.unload();
}

void ThermalRenderer::computeSceneAABB()
{
	RenderScene* _scene = Common::getInstance().getRenderSystem()->getMainScene();
	scene_s scene = _scene->getSceneData();

	if (scene.refModels.size() != 0) {
		min_aabb = glm::vec3(std::numeric_limits<float>::max());
		max_aabb = glm::vec3(std::numeric_limits<float>::min());

		for (RefModel_s* refModel : scene.refModels) {
			if (refModel->model->getName() == "Sky")
				continue;

			if (!((bool)refModel->model->getCustomProperty("traceable").getInt()))
				continue;

			glm::mat4 model_matrix = glm::transpose(refModel->model_matrix);
			for (RefMesh_s* refMesh : refModel->refMeshes) {
				Mesh* m = refMesh->mesh;
				for (int vi = 0; vi < m->getVertexCount(); vi++) {
					glm::vec4 v0_ = refModel->model_matrix * m->getVerticesVector()->at(vi).position;
					glm::vec3 v0 = glm::vec3(v0_.x, v0_.y, v0_.z) / v0_.w;
					min_aabb = glm::min(min_aabb, v0);
					max_aabb = glm::max(max_aabb, v0);
				}
			}
		}

		glm::vec3 half_diagonal = (max_aabb - min_aabb) * 0.5f;
		boundingSphereCenter = min_aabb + half_diagonal;
		boundingSphereRadius = glm::length(half_diagonal);

		spdlog::debug("AABB min: {}, {}, {}", min_aabb.x, min_aabb.y, min_aabb.z);
		spdlog::debug("AABB max: {}, {}, {}", max_aabb.x, max_aabb.y, max_aabb.z);

		spdlog::debug("boundingSphereCenter: {}, {}, {}", boundingSphereCenter.x, boundingSphereCenter.y, boundingSphereCenter.z);
		spdlog::debug("boundingSphereRadius: {}", boundingSphereRadius);
	}
}

glm::vec4 ThermalRenderer::getBoundingSphere()
{
	return glm::vec4(boundingSphereCenter.x, boundingSphereCenter.y, boundingSphereCenter.z, boundingSphereRadius);
}

// scene
void ThermalRenderer::sceneLoad(scene_s scene) {

	//scene_s s = _load_scene();
	_sceneLoad();
}

void ThermalRenderer::_sceneLoad() {

	computeSceneAABB();

	RenderScene* _scene = Common::getInstance().getRenderSystem()->getMainScene();
	scene_s scene = _scene->getSceneData();

	CHECK_EMPTY_SCENE(scene);

	loadData(scene);

	mThermalScene.load(scene);
	ThermalObjects& termObj = mThermalScene.getObjects();
	ThermalScene::SceneProperties_s& sceneProp = mThermalScene.getProperties();

	mThermalData.init(scene, termObj, sceneProp.vertexCount, sceneProp.triangleCount);
	mThermalData.load(scene, termObj, sky_min_kelvin);

	SingleTimeCommand stc = mGetStcBuffer();
	mThermalTransport.load(stc, mDevice, blas_gpu, mVkData->geometryDataBuffer, scene, mThermalScene, mThermalData, mThermalVars.batchCount, mThermalVars.rayCount, mThermalVars.rayDepth, solver.mode, true);
	computeTransportMatrix();

#ifndef DISABLE_GUI
	for (uint32_t si_idx = 0; si_idx < mFrameCount; si_idx++) {
		VkFrameData& frameData = mVkFrameData[si_idx];
		frameData.globalDescriptor.setBuffer(GLSL_GLOBAL_TRANSPORT_DATA_BINDING, &mThermalTransport.getTransportBuffer());
		frameData.globalDescriptor.setBuffer(GLSL_GLOBAL_KELVIN_DATA_BINDING, &mThermalTransport.getKelvinBuffer());
		frameData.globalDescriptor.update();
	}
#endif !DISABLE_GUI

	thermalInit(scene);

	printVramSize(mDevice);
}

void ThermalRenderer::loadData(scene_s scene)
{
	SingleTimeCommand stc = mGetStcBuffer();

	int count = 0;

#ifndef DISABLE_GUI
	td_gpu.loadScene(&stc, scene);
#endif !DISABLE_GUI

	// geometry lookup buffer to find the correct vertex informations in shader during ray tracing
	GeometrySSBO geometry[VK_GEOMETRY_SIZE] = {};
	int geometry_count = 0;
	int vertex_count = 0;
	int triangle_count = 0;

	blas_gpu.loadScene(&stc, scene);
	// no need to build any tlas if there are no models in the current scene
	spdlog::debug("model count = {}", scene.refModels.size());
	if (scene.refModels.size() != 0) {
		count = 0;
		for (RefModel_s* refModel : scene.refModels) {
			// each mesh in our model will be a geometry of this models blas
			for (RefMesh_s* refMesh : refModel->refMeshes) {
				Mesh* m = refMesh->mesh;

				GeometryDataVulkan::primitveBufferOffset_s offsets = blas_gpu.getOffset(m);
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
		mVkData->geometryDataBuffer.STC_UploadData(&stc, &geometry, geometry_count * sizeof(GeometrySSBO));

#ifndef DISABLE_GUI

		for (uint32_t si_idx = 0; si_idx < mFrameCount; si_idx++) {
			VkFrameData& frameData = mVkFrameData[si_idx];

			frameData.top.reserve(scene.refModels.size());
			for (RefModel_s* refModel : scene.refModels) {
				rvk::ASInstance as_instance(blas_gpu.getBlas(refModel->model));
				glm::mat4 model_matrix = glm::transpose(refModel->model_matrix);
				as_instance.setTransform(&model_matrix[0][0]);
				as_instance.setMask(refModel->mask);
				frameData.top.addInstance(as_instance);
			}
			frameData.top.prepare(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR);
			stc.begin();
			frameData.top.CMD_Build(stc.buffer());
			stc.end();
			// add the tlas to the descriptor and update it
			frameData.globalDescriptor.setAccelerationStructureKHR(GLSL_GLOBAL_AS_BINDING, &frameData.top);
		}

#endif !DISABLE_GUI

	}
}

void ThermalRenderer::computeTransportMatrix()
{
	if (!mDisableCompute)
	{
		SingleTimeCommand stc = mGetStcBuffer();
		mThermalTransport.compute(stc, mDevice, mThermalScene, mThermalData, mThermalVars.batchCount, mThermalVars.rayCount, mThermalVars.rayDepth, solver.mode);
		mThermalGui.autoAdjustDisplayRange(mThermalData.currentValueVector);		
	}
	mDisableCompute = false;
}

void ThermalRenderer::thermalInit(scene_s scene)
{			
	resetSimulation();
	spdlog::info("thermalInit: done");
}

void ThermalRenderer::getKelvin(float* _arr, unsigned int _count)
{
	const VectorXf tmp = mThermalData.currentValueVector.cast<FLOAT>();
	for (int i = 0; i < _count; i += 3)
	{
		float val = tmp[i / 3] / kelvinUnitFactor;
		_arr[i] = val;
		_arr[i + 1] = val;
		_arr[i + 2] = val;
	}
}

void ThermalRenderer::getValue(float* _arr, unsigned int _count, unsigned int _type)
{
	const VectorXf tmp = mThermalData.currentValueVector.cast<FLOAT>();
	Vec& vertexAreaVector = mThermalData.vertexAreaVector;
	for (int i = 0; i < _count; i += 3)
	{
		float val = tmp[i / 3] / kelvinUnitFactor;
		if (_type == 1)
			val = STEFAN_BOLTZMANN_CONST_RAW * vertexAreaVector[i / 3] * pow(val, 4.0);
		_arr[i] = val;
		_arr[i + 1] = val;
		_arr[i + 2] = val;
	}
}

void ThermalRenderer::setKelvin(float* _arr, unsigned int _arr_size, unsigned int _vertex_offset, bool _sky_values)
{
	Vec& currentValueVector = mThermalData.currentValueVector;

	if (_sky_values)
	{		
		RenderScene* _scene = Common::getInstance().getRenderSystem()->getMainScene();
		scene_s scene = _scene->getSceneData();

		Vec& vertexAreaVector = mThermalData.vertexAreaVector;
		SCALAR avg_sky_value = 0.0;
		for (RefModel_s* refModel : scene.refModels) {

			if (refModel->model->getName() == "Sky")
			{
				for (RefMesh_s* refMesh : refModel->refMeshes) {
					Mesh* m = refMesh->mesh;
					std::vector<uint32_t> vis = *m->getIndicesVector();
					unsigned int index_offset = 0;
					for (int i = 0; i < _arr_size; i++)
					{
						float sky_value = _arr[i]; // sky value in kWh/m2(/h)
						avg_sky_value += sky_value;

						{
							unsigned int vertex_ind_0 = sky_vertex_offset + vis[index_offset++];
							mThermalData.initialValueVector(vertex_ind_0) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_0], sky_min_kelvin);

							unsigned int vertex_ind_1 = sky_vertex_offset + vis[index_offset++];
							mThermalData.initialValueVector(vertex_ind_1) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_1], sky_min_kelvin);

							unsigned int vertex_ind_2 = sky_vertex_offset + vis[index_offset++];
							mThermalData.initialValueVector(vertex_ind_2) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_2], sky_min_kelvin);
						}

						{
							unsigned int vertex_ind_0 = sky_vertex_offset + vis[index_offset++];
							mThermalData.initialValueVector(vertex_ind_0) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_0], sky_min_kelvin);

							unsigned int vertex_ind_1 = sky_vertex_offset + vis[index_offset++];
							mThermalData.initialValueVector(vertex_ind_1) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_1], sky_min_kelvin);

							unsigned int vertex_ind_2 = sky_vertex_offset + vis[index_offset++];
							mThermalData.initialValueVector(vertex_ind_2) = sky_value_to_kelvin(sky_value, vertexAreaVector[vertex_ind_2], sky_min_kelvin);
						}
					}

					avg_sky_value /= ((float)_arr_size);
				}

				mThermalScene.getObjects().kelvin[0] = avg_sky_value;

				Vec sky_kelvin = mThermalData.initialValueVector.segment(sky_vertex_offset, sky_vertex_count);
				currentValueVector.segment(sky_vertex_offset, sky_vertex_count) = sky_kelvin;

#ifndef RUNTIME_OPTIMIZED
				spdlog::debug("sky_kelvin MIN: {}", sky_kelvin.minCoeff());
				spdlog::debug("sky_kelvin MEAN: {}", sky_kelvin.mean());
				spdlog::debug("sky_kelvin MAX: {}", sky_kelvin.maxCoeff());
#endif // !RUNTIME_OPTIMIZED
			}
		}
	}
	else
	{
		for (int i = 0; i < _arr_size; i++)
			currentValueVector[_vertex_offset+i] = _arr[i] * kelvinUnitFactor;
	}
}

void ThermalRenderer::setTimeStep(float _hours)
{
	solver.step_size = _hours * 3600.0 * secondsUnitFactor;
}

void ThermalRenderer::setTime(float _hours)
{
	mThermalVars.simulationTime = _hours * 3600.0 * secondsUnitFactor;
}


float ThermalRenderer::getTimeHours()
{
	return mThermalVars.simulationTime / (3600.0 * secondsUnitFactor);
}

void ThermalRenderer::resetSimulation()
{
	mThermalData.reset();
	mThermalVars.simulationTime = 0.0;

#ifndef DISABLE_GUI
	SingleTimeCommand stc = mGetStcBuffer();
	mThermalTransport.uploadValueVector(stc, mThermalData.currentValueVector);
	mThermalVars.simulationTime = 0.0;
	mThermalVars.remainingTimeSteps = 0;
	
	ThermalObjects& objects = mThermalScene.getObjects();
	mThermalData.setObjectStatistics(objects, mThermalData.currentValueVector);

	mThermalGui.autoAdjustDisplayRange(mThermalData.currentValueVector);
#endif !DISABLE_GUI
}

void ThermalRenderer::thermalTimestep()
{
	Vec x = mThermalData.currentValueVector;
	
	if ((solver.mode == 0) && solver.compute_steady_state)
	{
		//SCALAR mean = x.mean();
		x = (mThermalData.fixedVarsVector.array() < 1.0 && x.array() == 0.0).select(1.0, x);
	}

	if (!solver.compute_steady_state && mThermalVars.simulationTime <= 0.0)
	{
		spdlog::info("time: {} h | step dur.: {} s", 0, 0);
		logEigenBase("x", x.transpose() / kelvinUnitFactor);
	}

	spdlog::stopwatch sw;

	logEigenBase("thermalVars.initialValueVector", mThermalData.initialValueVector.transpose() / kelvinUnitFactor);
	logEigenBase("solve(x)", x.transpose() / kelvinUnitFactor);

#ifndef RUNTIME_OPTIMIZED
	SCALAR val = solver.residual(x).array().pow(2.0).sum();
	spdlog::debug("value(x) = {}", val);
#endif // !RUNTIME_OPTIMIZED

	solver.solve(x, mThermalData.currentValueVector, mThermalTransport.getTransportMatrix(), mThermalData.fixedVarsVector);

	if (solver.mode == 1)
	{		
		x = (mThermalData.fixedVarsVector.array() < 1.0).select(x, mThermalData.initialValueVector);
	}

	mThermalData.currentValueVector = x;

	if (solver.compute_steady_state) {
		spdlog::info("thermalTimestep() - steady state | dur. {:.3} s", sw);
	}		
	else {
		mThermalVars.simulationTime += solver.step_size;
		spdlog::info("thermalTimestep() - time: {} h | dur. {:.3} s", mThermalVars.simulationTime / (secondsUnitFactor * 3600.0), sw);
	}	
	logEigenBase("x (kelvin)", x.transpose() / kelvinUnitFactor);

#ifndef DISABLE_GUI

	SingleTimeCommand stc = mGetStcBuffer();
	mThermalTransport.uploadValueVector(stc, mThermalData.currentValueVector);

	if (mThermalGui.showStatistics)
	{
		ThermalObjects& objects = mThermalScene.getObjects();
		mThermalData.setObjectStatistics(objects, mThermalData.currentValueVector);
	}

	mThermalGui.autoAdjustDisplayRange(mThermalData.currentValueVector);
#endif !DISABLE_GUI
}

void ThermalRenderer::sceneUnload(scene_s scene) {

	blas_gpu.unloadScene();

#ifndef DISABLE_GUI

	td_gpu.unloadScene();
	for (int i = 0; i < mFrameCount; i++) {
		mVkFrameData[i].top.destroy();
	}

#endif // !DISABLE_GUI

	solver.reset();
	mThermalTransport.unload();
	mThermalScene.unload();
	mThermalData.unload();
}

void ThermalRenderer::_sceneUnload() {
	RenderScene* scene = Common::getInstance().getRenderSystem()->getMainScene();
	scene->readyToRender(false);
	sceneUnload(scene->getSceneData());
	for (auto m : scene->getSceneData().refModels)
		scene->removeModel(m);
	for (auto m : scene->getSceneData().models)
		delete m;
	scene->getSceneData().models.clear();
}

// frame
void ThermalRenderer::drawView(viewDef_s* aViewDef) {

#ifdef DISABLE_GUI
	assert(false);// , "drawView called!");
#endif // DISABLE_GUI

	SingleTimeCommand stc = mGetStcBuffer();

	if (mThermalVars.recomputeTransport)
	{
		resetSimulation();
		RenderScene* render_scene = tamashii::Common::getInstance().getRenderSystem()->getMainScene();
		mThermalTransport.load(stc, mDevice, blas_gpu, mVkData->geometryDataBuffer, render_scene->getSceneData(), mThermalScene, mThermalData, mThermalVars.batchCount, mThermalVars.rayCount, mThermalVars.rayDepth, solver.mode);
		computeTransportMatrix();
		mThermalTransport.uploadValueVector(stc, mThermalData.currentValueVector);
		mThermalVars.recomputeTransport = false;
	}

	CommandBuffer* cb = mGetCurrentCmdBuffer();
	if (!aViewDef->surfaces.size()) {
		const glm::vec4 cc = glm::vec4(var::bg.getInt3(), 255.f) / 255.0f;
		mSwapchain->CMD_BeginRenderClear(cb, { { cc.x, cc.y, cc.z, 1.0f }, {0,0} });
		mSwapchain->CMD_EndRender(cb);
		return;
	};

	// ubo
	GlobalUbo ubo;
	std::memcpy(&ubo.viewMat, &aViewDef->view_matrix[0], sizeof(glm::mat4));
	std::memcpy(&ubo.projMat, &aViewDef->projection_matrix[0], sizeof(glm::mat4));
	std::memcpy(&ubo.inverseViewMat, &aViewDef->inv_view_matrix[0], sizeof(glm::mat4));
	std::memcpy(&ubo.inverseProjMat, &aViewDef->inv_projection_matrix[0], sizeof(glm::mat4));
	std::memcpy(&ubo.viewPos, &aViewDef->view_pos[0], sizeof(glm::vec3));
	std::memcpy(&ubo.viewDir, &aViewDef->view_dir[0], sizeof(glm::vec3));
	mVkFrameData[mCurrentFrame()].globalUniformBuffer.STC_UploadData(&stc, &ubo, sizeof(GlobalUbo));

	if (mThermalVars.reset)
	{
		resetSimulation();
		mThermalVars.reset = false;
	}

	if (mThermalVars.remainingTimeSteps > 0)
	{
		thermalTimestep();
		if (solver.compute_steady_state)
			mThermalVars.remainingTimeSteps = 0;
		else
			mThermalVars.remainingTimeSteps -= 1;
	}

	// aux ubo
	AuxiliaryUbo aux_ubo;
	aux_ubo.vertexCount = mThermalScene.getProperties().vertexCount;
	aux_ubo.sourceVertexInd = mThermalGui.sourceVertexId;
	aux_ubo.visualization = mThermalGui.visualization;
	aux_ubo.displayRangeMin = mThermalGui.displayRangeMin;
	aux_ubo.displayRangeMax = mThermalGui.displayRangeMax;
	aux_ubo.clipDistance = mThermalGui.clipDistance;
	aux_ubo.backfaceCulling = mThermalGui.backfaceCulling;
	mVkFrameData[mCurrentFrame()].globalAuxUniformBuffer.STC_UploadData(&stc, &aux_ubo, sizeof(AuxiliaryUbo));

	// lights
	LightsSSBO lights;
	int count = 0;
	for (const RefLight_s* l : aViewDef->lights) {
		PunctualLight& pl = lights.punctual_lights[count];
		// TODO: don't use enum type direct
		pl.type = (int)l->light->getType();
		std::memcpy(&pl.color, &l->light->getColor()[0], sizeof(glm::vec3));
		std::memcpy(&pl.position, &l->position[0], sizeof(glm::vec3));
		std::memcpy(&pl.direction, &l->direction[0], sizeof(glm::vec3));
		pl.intensity = l->light->getIntensity();
		switch (l->light->getType()) {
		case Light::Type::POINT:
			pl.range = ((PointLight*)l->light)->getRange();
			break;
		case Light::Type::SPOT:
			pl.range = ((SpotLight*)l->light)->getRange();
			pl.lightAngleScale = 1.0f / std::max(0.001f, (float)(cos(((SpotLight*)l->light)->getInnerConeAngle()) - cos(((SpotLight*)l->light)->getOuterConeAngle())));
			pl.lightAngleOffset = -cos(((SpotLight*)l->light)->getOuterConeAngle()) * pl.lightAngleScale;
			break;
		}
		count++;
	}
	lights.punctual_light_count = count;
	mVkData->pointLightBuffer.STC_UploadData(&stc, &lights, sizeof(LightsSSBO));

	InstanceSSBO instance[VK_INSTANCE_SIZE] = {};

	if (aViewDef->scene.refModels.size() != 0) {
		VkFrameData& frameData = mVkFrameData[mCurrentFrame()];
		frameData.top.clear();
		//frameData.top.destroy();
		//frameData.top.reserve(vd->refModels->size());

		int ind = 0;
		int geometry_offset = 0;
		ThermalObjects thermalObjects = mThermalScene.getObjects();
		for (RefModel_s *refModel : aViewDef->scene.refModels) {
			// glm stores matrices in a column-major order but vulkan acceleration structures requiere a row-major order
			glm::mat4 model_matrix = glm::transpose(refModel->model_matrix);
			// add instance infos for our instance lookup buffer
			
			//int ind = frameData.top.size();
			
			instance[ind].geometry_buffer_offset = geometry_offset;
			geometry_offset += refModel->refMeshes.size();
			std::memcpy(&instance[ind].model_matrix, &refModel->model_matrix[0], sizeof(glm::mat4));

			glm::mat4 trp_inv_model_matrix = glm::transpose(glm::inverse(refModel->model_matrix));
			std::memcpy(&instance[ind].trp_inv_model_matrix, &trp_inv_model_matrix[0], sizeof(glm::mat4));

			instance[ind].diffuseReflectance = thermalObjects.diffuseReflectance[ind];
			instance[ind].specularReflectance = thermalObjects.specularReflectance[ind];
			instance[ind].absorption = thermalObjects.absorption[ind];
			instance[ind].diffuseEmission = thermalObjects.diffuseEmission[ind];
			instance[ind].traceable = thermalObjects.traceable[ind];

			// create an instance for the blas
			rvk::ASInstance as_instance(blas_gpu.getBlas(refModel->model));
			as_instance.setTransform(&model_matrix[0][0]);
			as_instance.setMask(refModel->mask);
			// add the blas to the tlas
			frameData.top.addInstance(as_instance);
			ind++;
		}

		frameData.top.CMD_Build(cb, &frameData.top);
		cb->cmdMemoryBarrier(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR);

		// upload instance
		frameData.instanceDataBuffer.STC_UploadData(&stc, &instance, frameData.top.size() * sizeof(InstanceSSBO));
		frameData.instanceDataBuffer.CMD_BufferMemoryBarrier(cb, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR);

		mVkData->rtpipeline.CMD_BindDescriptorSets(cb, { &mVkFrameData[mCurrentFrame()].globalDescriptor, td_gpu.getDescriptor()});
		mVkData->rtpipeline.CMD_BindPipeline(cb);
		mVkData->rtpipeline.CMD_TraceRays(cb, aViewDef->target_size.x, aViewDef->target_size.y);
	}

	rvk::swapchain::CMD_BlitImageToCurrentSwapchainImage(cb, mSwapchain, &mVkFrameData[mCurrentFrame()].rtImage, VK_FILTER_LINEAR);
}

void ThermalRenderer::drawUI(uiConf_s* uc)
{
	mThermalGui.draw(mThermalScene.getProperties().vertexCount, solver, mThermalVars, mThermalData, mThermalScene.getObjects());
}
