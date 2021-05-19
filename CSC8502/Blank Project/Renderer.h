#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Camera.h"

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
};
