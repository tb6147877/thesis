#pragma once
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
	Vector4 position;
	Vector4 radius;
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
		Debug_Depth,
		Debug_Lights
	};


	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;

	 void SwitchShadingType(const ShadingType type) { m_shadingType = type; }
	 void SetExposure(const float x) { m_exposure += x; }
protected:
	const unsigned int NUM_LIGHTS = 6;

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
	ComputeShader* m_lightCullingShader;
	void DepthPrePass();
	void LightCullingPass();
	void CalculateLighting();

};
