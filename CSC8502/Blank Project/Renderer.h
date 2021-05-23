#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Assimp_Model.h"
#include "../nclgl/Light.h"
#include "../nclgl/Quad.h"
#include "../nclgl/Sphere.h"
#include "DeferredRenderingHelper.h"

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
protected:
	ShadingType m_shadingType;
	Camera* m_camera;
	Assimp_Model* m_model;
	std::vector<Light*> m_lights;
	Quad* m_quad;
	Sphere* m_sphere;
	DeferredRenderingHelper* m_deferredHelper;

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
};
