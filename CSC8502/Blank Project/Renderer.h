#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Assimp_Model.h"
#include "../nclgl/Light.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;
protected:

	Camera* m_camera;
	Assimp_Model* m_model;
	Shader* m_modelShader;
	std::vector<Light*> m_lights;

	void GenerateLights();
};
