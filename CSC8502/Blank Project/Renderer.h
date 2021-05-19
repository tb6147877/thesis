#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Assimp_Model.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;
protected:
	Mesh*	triangle;
	Shader* basicShader;


	Camera* m_camera;
	Assimp_Model* m_model;
	Shader* m_modelShader;
};
