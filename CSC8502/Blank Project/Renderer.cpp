#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	m_camera = new Camera(0.0f, 0.0f, Vector3{ 0.0f,0.0f,0.0f });
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	m_model = new Assimp_Model("sponza.obj");

	//m_modelShader = new Shader("ModelBasicVert.glsl", "ModelBasicFrag.glsl");
	m_modelShader = new Shader("ForwardVert.glsl", "ForwardFrag.glsl");

	if (!m_modelShader->LoadSuccess()) {
		return;
	}

	GenerateLights();

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	init = true;
}

Renderer::~Renderer(void)	{
	delete m_camera;
	delete m_model;
	delete m_modelShader;

	for (int i = 0; i < m_lights.size(); i++)
	{
		delete m_lights[i];
	}
}

void Renderer::GenerateLights() {
	//point lights
	for (int i = 0; i < 6; i++)
	{
		m_lights.push_back(new Light{ Light::LightType::Point, Vector3{i*300.0f,120.0f,0.0f},200.0f,Vector3{1.0f,1.0f,1.0f} });
	}
}


void Renderer::UpdateScene(float dt) {
	m_camera->UpdateCamera(dt);
	viewMatrix = m_camera->BuildViewMatrix();
}

void Renderer::RenderScene()	{
	
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	BindShader(m_modelShader);
	UpdateShaderMatrices();

	glUniform3fv(glGetUniformLocation(m_modelShader->GetProgram(), "viewPos"), 1, (float*)&m_camera->GetPosition());
	for (int i = 0; i < m_lights.size(); i++)
	{
		std::string temp{ "pointLights[" + std::to_string(i) + "]" };
		
		glUniform3fv(glGetUniformLocation(m_modelShader->GetProgram(), temp.append(".color").c_str() ), 1, (float*)&m_lights[i]->GetColor());
		temp = "pointLights[" + std::to_string(i) + "]";
		glUniform3fv(glGetUniformLocation(m_modelShader->GetProgram(),  temp.append(".position").c_str() ), 1, (float*)&m_lights[i]->GetPosition());
		temp = "pointLights[" + std::to_string(i) + "]";
		glUniform1f(glGetUniformLocation(m_modelShader->GetProgram(), temp.append(".radius").c_str() ), m_lights[i]->GetRadius());
	}

	m_model->Draw(m_modelShader);
}

