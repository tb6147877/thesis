#pragma once

#include <random>

#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Assimp_Model.h"
#include "../nclgl/Light.h"
#include "../nclgl/Quad.h"
#include "../nclgl/Sphere.h"
#include "../nclgl/ComputeShader.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/FileOperator.h"
#include "DeferredRenderingHelper.h"
#include "DepthPreHelper.h"
#include "FinalOutputHelper.h"
#include "SelectActiveClusterHelper.h"
#include "MSAAHelper.h"

struct PointLight
{
	Vector4 color;
	Vector4 position_radius;
};

struct VisibleIndex
{
	int index;
};

struct ClusterAABB
{
	Vector4 minPoint;
	Vector4 maxPoint;
};

struct ClusterBasic
{
	Matrix4 inverseProjection;
	unsigned int clusterSizes[4];
	unsigned int screenWidth;
	unsigned int screenHeight;
	float sliceScalingFac;
	float sliceBiasFac;
};

class Renderer : public OGLRenderer	{
public:
	enum ShadingType {
		Forward,
		Deferred,
		ForwardPlus,
		Cluster,
		ForwardPlus_Debug_Depth,
		ForwardPlus_Debug_Lights
	};


	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;

	 void SwitchShadingType(const ShadingType type) { m_shadingType = type; }
	 void SetExposure(const float x) { m_exposure += x; }
	 void SetShowSlice(const int flag) { m_showSlices = flag; }
	 int GetFPS() { return m_fps; }
	 std::string GetShadingTypeStr();

protected:
	const unsigned int NUM_LIGHTS = 800;
	const int MAX_NUM_LIGHTS = 2048;
	const float LIGHT_RADIUS = 120.0f;
	const GLuint MAX_LIGHT_NUMBER_PER_CLUSTER = 100;
	const int TILE_SIZE = 8;
	const float LIGHT_BORDER_MIN[3]={ -1400.0f, -100.0f, -640.0f};
	const float LIGHT_BORDER_MAX[3]={ 1300.0f,1400.0f, 560.0f };
	/*const float LIGHT_BORDER_MIN[3]={ -140.0f, -10.0f, -64.0f};
	const float LIGHT_BORDER_MAX[3]={ 130.0f,140.0f, 56.0f };*/
	
	int m_fps{0};
	ShadingType m_shadingType;
	Camera* m_camera;
	Assimp_Model* m_model;
	std::vector<Light*> m_lights;
	Quad* m_quad;
	Sphere* m_sphere;
	DeferredRenderingHelper* m_deferredHelper;
	DepthPreHelper* m_depthPreHelper;
	FinalOutputHelper* m_finalHelper;
	Shader* m_finalShader;
	Frustum* m_frustum;
	float m_exposure;
	std::vector<int> m_performanceData;

	void GenerateLights();
	void UpdateLights(const float dt);
	Vector3 RandomLightPosition(std::uniform_real_distribution<> dis, std::mt19937 gen);
	int CalculateFPS(const float dt);
	void DepthPrePass(const bool isMSAA=false);
	void RecordPerformaceData();
	

	//Forward Shading part
	MSAAHelper* m_msaahelper;
	Shader* m_modelShader;
	void ForwardRendering(const bool isMSAA = false);

	//Deferred Shaing part
	Shader* m_fillBufferShader;
	Shader* m_lightingShader;
	Shader* m_combineShader;
	void FillGBuffer();
	void RenderLighting();
	void CombineBuffer();

	//forward+ part
	GLuint m_workGroupsX, m_workGroupsY;
	GLuint m_lightsSSBO, m_visibleLightIndicesSSBO;
	Shader* m_depthPreShader;
	Shader* m_fp_lightingShader;
	Shader* m_fp_depthDebugShader;
	Shader* m_fp_lightDebugShader;
	ComputeShader* m_lightCullingShader;
	void InitForwardPlusRendering();
	void LightCullingPass();
	void CalculateLighting();
	void DrawDepthDebug();
	void DrawLightDebug();
	void FillLightsSSBO();
	void GetComputeShaderLimit();

	//cluster part
	int m_showSlices {0};
	unsigned int m_frameIndex{ 1 };
	

	//this cluster size must fit resolution of render target, eg. the render target is 16:9, so x is 16, y is 9
	//1280:720 fit this resolution
	/*const GLuint CLUSTER_SIZE_X = 16;
	const GLuint CLUSTER_SIZE_Y = 9;
	const GLuint CLUSTER_SIZE_Z = 24;*/
	const GLuint CLUSTER_SIZE_X =40;
	const GLuint CLUSTER_SIZE_Y = 23;
	const GLuint CLUSTER_SIZE_Z = 61;
	float m_near=1.0f, m_far=3000.0f;
	GLuint m_clusterNumber = CLUSTER_SIZE_X * CLUSTER_SIZE_Y * CLUSTER_SIZE_Z;
	GLuint m_clusterAABBSSBO, m_clusterBasicSSBO, m_lightGridsSSBO, 
				m_globalLightIndexCountSSBO, m_lightIndexListSSBO, m_activeClusterListSSBO;
	GLuint m_activeClustersCountSSBO,m_activeClustersSSBO,m_denseActiveClustersSSBO;
	ComputeShader* m_c_generateClusterShader;
	ComputeShader* m_c_lightCullingShader;
	Shader* m_c_lightingShader, *m_c_selectActiveShader;
	ClusterBasic m_clusterBaisc;
	SelectActiveClusterHelper* m_selectActiveClusterHelper;
	ComputeShader* m_c_buildDenseClustersListShader;
	unsigned int m_activeClustersCount;

	void InitClusterRendering();
	void SelectActiveClusters();
	void ClusterLightCulling();
	void ClusterCalculateLighting();

	
};
