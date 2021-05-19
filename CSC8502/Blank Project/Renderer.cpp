#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	m_camera = new Camera(0.0f, 0.0f, Vector3{ 0.0f,0.0f,0.0f });
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	m_model = new Assimp_Model("sponza.obj");

	m_modelShader = new Shader("ModelBasicVert.glsl", "ModelBasicFrag.glsl");

	if (!m_modelShader->LoadSuccess()) {
		return;
	}

	/*triangle = Mesh::GenerateTriangle();

	basicShader = new Shader("MatrixVertex.glsl","colourFragment.glsl");

	if(!basicShader->LoadSuccess()) {
		return;
	}*/

	glEnable(GL_DEPTH_TEST);
	init = true;
}
Renderer::~Renderer(void)	{
	delete m_camera;
	delete m_model;
	/*delete triangle;
	delete basicShader;*/
}

void Renderer::UpdateScene(float dt) {
	m_camera->UpdateCamera(dt);
	viewMatrix = m_camera->BuildViewMatrix();
}

void Renderer::RenderScene()	{
	
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	BindShader(m_modelShader);
	modelMatrix = Matrix4::Scale(Vector3{ 1.0f,1.0f,1.0f });


	UpdateShaderMatrices();
	m_model->Draw(m_modelShader);


	/*BindShader(basicShader);

	UpdateShaderMatrices();

	triangle->Draw();*/
}

