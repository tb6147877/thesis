#pragma once

#include <random>

#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Assimp_Model.h"
#include "../nclgl/Light.h"
#include "../nclgl/Quad.h"
#include "../nclgl/Sphere.h"
#include "../nclgl/ComputeShader.h"
#include "DeferredRenderingHelper.h"
#include "DepthPreHelper.h"
#include "FinalOutputHelper.h"

struct PointLight
{
	Vector4 color;
	Vector4 position_radius;
};

struct VisibleIndex
{
	int index;
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
protected:
	const unsigned int NUM_LIGHTS = 1000;
	const float LIGHT_RADIUS = 100.0f;
	const float LIGHT_BORDER_MIN[3]={ -1400.0f, 0.0f, -640.0f};
	const float LIGHT_BORDER_MAX[3]={ 1300.0f,1400.0f, 560.0f };
	
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
	float m_exposure;

	void GenerateLights();
	Vector3 RandomLightPosition(std::uniform_real_distribution<> dis, std::mt19937 gen);
	

	//Forward Shading part
	Shader* m_modelShader;
	void ForwardRendering();

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
	void DepthPrePass();
	void LightCullingPass();
	void CalculateLighting();
	void DrawDepthDebug();
	void DrawLightDebug();
};
