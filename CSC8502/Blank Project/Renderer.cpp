#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	m_camera = new Camera(0.0f, 0.0f, Vector3{ 0.0f,0.0f,0.0f });
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);


	triangle = Mesh::GenerateTriangle();

	basicShader = new Shader("MatrixVertex.glsl","colourFragment.glsl");

	if(!basicShader->LoadSuccess()) {
		return;
	}

	init = true;
}
Renderer::~Renderer(void)	{
	delete m_camera;
	delete triangle;
	delete basicShader;
}

void Renderer::UpdateScene(float dt) {
	m_camera->UpdateCamera(dt);
	viewMatrix = m_camera->BuildViewMatrix();
}

void Renderer::RenderScene()	{
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(basicShader);

	UpdateShaderMatrices();


	triangle->Draw();
}

