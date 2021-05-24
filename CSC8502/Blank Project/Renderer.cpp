#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	m_shadingType = ShadingType::ForwardPlus;

	m_camera = new Camera(0.0f, 0.0f, Vector3{ 0.0f,0.0f,0.0f });
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	m_model = new Assimp_Model("sponza.obj");
	m_quad = new Quad();
	m_sphere = new Sphere();
	m_deferredHelper = new DeferredRenderingHelper{ width,height };
	m_depthPreHelper =new DepthPreHelper{ width,height };

	//m_modelShader = new Shader("ModelBasicVert.glsl", "ModelBasicFrag.glsl");
	m_modelShader = new Shader("ForwardVert.glsl", "ForwardFrag.glsl");

	m_fillBufferShader = new Shader("Deferred_FillBufferVert.glsl", "Deferred_FillBufferFrag.glsl");
	m_lightingShader = new Shader("Deferred_LightingVert.glsl", "Deferred_LightingFrag.glsl");
	m_combineShader = new Shader("Deferred_CombineVert.glsl", "Deferred_CombineFrag.glsl");
	
	m_depthPreShader= new Shader("ForwardPlus_DepthPreVert.glsl", "ForwardPlus_DepthPreFrag.glsl");
	m_lightCullingShader = new ComputeShader("ForwardPlus_LightCullingComp.glsl");

	if (!m_modelShader->LoadSuccess() 
		|| !m_fillBufferShader->LoadSuccess() 
		|| !m_lightingShader->LoadSuccess() 
		|| !m_combineShader->LoadSuccess()
		|| !m_depthPreShader->LoadSuccess()
		) {
		return;
	}

	GenerateLights();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	init = true;
}

Renderer::~Renderer(void)	{
	delete m_camera;
	delete m_model;
	delete m_modelShader;
	delete m_deferredHelper;
	delete m_quad;
	delete m_sphere;
	delete m_depthPreHelper;

	delete m_fillBufferShader;
	delete m_lightingShader;
	delete m_combineShader;

	delete m_depthPreShader;
	delete m_lightCullingShader;

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

	switch (m_shadingType)
	{
	case Renderer::Forward:
		//0.Forward Rendering
		ForwardRendering();
		break;
	case Renderer::Deferred:
		//1.Deferred Rendering
		FillGBuffer();
		RenderLighting();
		CombineBuffer();
		break;
	case Renderer::ForwardPlus:
		//2.Forward+ Rendering
		DepthPrePass();
		break;
	case Renderer::Cluster:
		break;
	case Renderer::Debug_Depth:
		break;
	case Renderer::Debug_Lights:
		break;
	default:
		break;
	}

}

void Renderer::ForwardRendering(){
	BindShader(m_modelShader);
	UpdateShaderMatrices();
	glUniform3fv(glGetUniformLocation(m_modelShader->GetProgram(), "viewPos"), 1, (float*)&m_camera->GetPosition());
	for (int i = 0; i < m_lights.size(); i++)
	{
		std::string temp{ "pointLights[" + std::to_string(i) + "]" };

		glUniform3fv(glGetUniformLocation(m_modelShader->GetProgram(), temp.append(".color").c_str()), 1, (float*)&m_lights[i]->GetColor());
		temp = "pointLights[" + std::to_string(i) + "]";
		glUniform3fv(glGetUniformLocation(m_modelShader->GetProgram(), temp.append(".position").c_str()), 1, (float*)&m_lights[i]->GetPosition());
		temp = "pointLights[" + std::to_string(i) + "]";
		glUniform1f(glGetUniformLocation(m_modelShader->GetProgram(), temp.append(".radius").c_str()), m_lights[i]->GetRadius());
	}
	m_model->Draw(m_modelShader);
}

void Renderer::FillGBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_deferredHelper->GetGBufferFBO());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(m_fillBufferShader);
	UpdateShaderMatrices();

	m_model->Draw(m_fillBufferShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderLighting() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_deferredHelper->GetLightingFBO());
	BindShader(m_lightingShader);
	UpdateShaderMatrices();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(m_lightingShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_deferredHelper->GetGBufferDepthTex());

	glUniform1i(glGetUniformLocation(m_lightingShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_deferredHelper->GetGBufferNormalTex());

	glUniform2f(glGetUniformLocation(m_lightingShader->GetProgram(), "pixelSize"), 1.0f / (float)width, 1.0f / (float)height);
	glUniform3fv(glGetUniformLocation(m_lightingShader->GetProgram(), "viewPos"), 1, (float*)&m_camera->GetPosition());

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(m_lightingShader->GetProgram(), "inverseProjView"), 1, false, (float*)&invViewProj);

	for (int i = 0; i < m_lights.size(); i++)
	{
		glUniform1f(glGetUniformLocation(m_lightingShader->GetProgram(), "lightRadius"), m_lights[i]->GetRadius());
		glUniform3fv(glGetUniformLocation(m_lightingShader->GetProgram(), "lightPos"), 1, (float*)&m_lights[i]->GetPosition());

		glUniform1f(glGetUniformLocation(m_lightingShader->GetProgram(), "pointLight.radius"), m_lights[i]->GetRadius());
		glUniform3fv(glGetUniformLocation(m_lightingShader->GetProgram(), "pointLight.position"), 1, (float*)&m_lights[i]->GetPosition());
		glUniform3fv(glGetUniformLocation(m_lightingShader->GetProgram(), "pointLight.color"), 1, (float*)&m_lights[i]->GetColor());

		m_sphere->Draw();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineBuffer() {
	BindShader(m_combineShader);

	glUniform1i(glGetUniformLocation(m_combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_deferredHelper->GetGBufferColorTex());

	glUniform1i(glGetUniformLocation(m_combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_deferredHelper->GetLightingDiffTex());

	glUniform1i(glGetUniformLocation(m_combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_deferredHelper->GetLightingSpecTex());

	m_quad->Draw();
}


void Renderer::DepthPrePass(){
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthPreHelper->GetFBO());
	glClear(GL_DEPTH_BUFFER_BIT);
	BindShader(m_depthPreShader);
	UpdateShaderMatrices();
	m_model->Draw(m_depthPreShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

