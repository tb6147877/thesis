#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	//GetComputeShaderLimit();
	m_shadingType = ShadingType::Cluster;
	m_exposure = 1.0f;
	m_camera = new Camera(0.0f, 90.0f, Vector3{ 1100.0f,100.0f,0.0f });
	projMatrix = Matrix4::Perspective(m_near, m_far, (float)width / (float)height, 45.0f);

	m_model = new Assimp_Model("sponza.obj");
	m_quad = new Quad();
	m_sphere = new Sphere();
	m_deferredHelper = new DeferredRenderingHelper{ width,height };
	m_depthPreHelper =new DepthPreHelper{ width,height };
	m_finalHelper=new FinalOutputHelper{ width,height };
	m_frustum = new Frustum{};

	//m_modelShader = new Shader("ModelBasicVert.glsl", "ModelBasicFrag.glsl");
	m_modelShader = new Shader("ForwardVert.glsl", "ForwardFrag.glsl");
	m_finalShader = new Shader("FinalVert.glsl", "FinalFrag.glsl");

	m_fillBufferShader = new Shader("Deferred_FillBufferVert.glsl", "Deferred_FillBufferFrag.glsl");
	m_lightingShader = new Shader("Deferred_LightingVert.glsl", "Deferred_LightingFrag.glsl");
	m_combineShader = new Shader("Deferred_CombineVert.glsl", "Deferred_CombineFrag.glsl");
	
	m_depthPreShader= new Shader("ForwardPlus_DepthPreVert.glsl", "ForwardPlus_DepthPreFrag.glsl");
	m_lightCullingShader = new ComputeShader("ForwardPlus_LightCullingComp.glsl");
	m_fp_lightingShader = new Shader("ForwardPlus_LightingVert.glsl", "ForwardPlus_LightingFrag.glsl");
	m_fp_depthDebugShader = new Shader("DepthDebugVert.glsl","DepthDebugFrag.glsl");
	m_fp_lightDebugShader = new Shader("ForwardPlus_LightingDebugVert.glsl", "ForwardPlus_LightingDebugFrag.glsl");

	m_c_generateClusterShader = new ComputeShader("Cluster_GenerateClusterComp.glsl");
	m_c_lightingShader = new Shader("Cluster_LightingVert.glsl","Cluster_LightingFrag.glsl");
	m_c_lightCullingShader = new ComputeShader("Cluster_LightCullingComp.glsl");

	//modelMatrix = Matrix4::Scale(Vector3{ 0.1f,0.1f,0.1f });

	if (!m_modelShader->LoadSuccess() 
		|| !m_finalShader->LoadSuccess()
		|| !m_fillBufferShader->LoadSuccess() 
		|| !m_lightingShader->LoadSuccess() 
		|| !m_combineShader->LoadSuccess()
		|| !m_depthPreShader->LoadSuccess()
		|| !m_fp_lightingShader->LoadSuccess()
		|| !m_fp_depthDebugShader->LoadSuccess()
		|| !m_fp_lightDebugShader->LoadSuccess()
		|| !m_c_lightingShader->LoadSuccess()
		) {
		return;
	}

	InitForwardPlusRendering();
	GenerateLights();
	InitClusterRendering();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	init = true;
}

Renderer::~Renderer(void)	{
	RecordPerformaceData();

	delete m_camera;
	delete m_model;
	delete m_modelShader;
	delete m_deferredHelper;
	delete m_quad;
	delete m_sphere;
	delete m_depthPreHelper;
	delete m_finalHelper;
	delete m_finalShader;
	delete m_frustum;

	delete m_fillBufferShader;
	delete m_lightingShader;
	delete m_combineShader;

	delete m_depthPreShader;
	delete m_lightCullingShader;
	delete m_fp_lightingShader;
	delete m_fp_depthDebugShader;
	delete m_fp_lightDebugShader;

	delete m_c_generateClusterShader;
	delete m_c_lightingShader;
	delete m_c_lightCullingShader;

	for (int i = 0; i < m_lights.size(); i++)
	{
		delete m_lights[i];
	}
}

void Renderer::GetComputeShaderLimit() {
	int dim = 0; /* 0=x, 1=y, 2=z */
	int maxSizeX,maxSizeY,maxSizeZ;
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxSizeX);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxSizeY);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxSizeZ);

	int maxInvoc;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxInvoc);
}

int Renderer::CalculateFPS(const float dt) {
	static int fps = 0;
	static float lastTime = 0; 
	static int frameCount = 0;

	++frameCount;

	lastTime +=dt;
	if (lastTime>=1.0f)
	{
		fps = frameCount;
		if (fps > 2)
		{
			m_performanceData.push_back(fps);
		}
		frameCount = 0;
		lastTime -= 1.0f;
	}
	return fps;
	
}

void Renderer::RecordPerformaceData() {
	std::string temp{""};
	for (size_t i = 0; i < m_performanceData.size(); i++)
	{
		temp += (std::to_string(m_performanceData[i]) + ",\n");
	}
	FileOperator::writeFile(temp, "C:/Users/tb614/Desktop/cluster.csv");
}


std::string Renderer::GetShadingTypeStr() {
	switch (m_shadingType)
	{
	case Renderer::Forward:
		return "Forward";
	case Renderer::Deferred:
		return "Deferred";
	case Renderer::ForwardPlus:
		return "Forward+";
	case Renderer::Cluster:
		return "Cluster";
	case Renderer::ForwardPlus_Debug_Depth:
		return "Depth Debug";
	case Renderer::ForwardPlus_Debug_Lights:
		return "Tiled Debug";
	default:
		return "";
	}
}

void Renderer::GenerateLights() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);
	std::uniform_real_distribution<> disColor(0, 8);
	//point lights
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		//colorful lights
		m_lights.push_back(new Light{ Light::LightType::Point, RandomLightPosition(dis,gen),LIGHT_RADIUS,Vector3{1.0f+(float)disColor(gen),1.0f + (float)disColor(gen),1.0f + (float)disColor(gen)} });
		
		//white lights
		//m_lights.push_back(new Light{ Light::LightType::Point, RandomLightPosition(dis,gen),LIGHT_RADIUS,Vector3{1.0f ,1.0f,1.0f} });
		//dis(gen);//must has this line, or the random position is same
	}

	FillLightsSSBO();
}

void Renderer::UpdateLights(const float dt) {
	for (int i = 0; i < NUM_LIGHTS; i++) {
		float min = LIGHT_BORDER_MIN[1];
		float max = LIGHT_BORDER_MAX[1];

		Vector3 temp = m_lights[i]->GetPosition();
		temp.y= fmod((temp.y + (-10.0f * dt) - min + max), max) + min;
		//temp.y = fmod((temp.y + (-1.0f * dt) - min + max), max) + min;
		m_lights[i]->SetPosition(temp);
	}

	FillLightsSSBO();
}

void Renderer::FillLightsSSBO(){
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightsSSBO);
	PointLight* pointLights = (PointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	for (int i = 0; i < m_lights.size(); i++)
	{
		PointLight& light = pointLights[i];
		light.color = Vector4{ m_lights[i]->GetColor().x,m_lights[i]->GetColor().y,m_lights[i]->GetColor().z,1.0f };
		light.position_radius = Vector4{ m_lights[i]->GetPosition().x,m_lights[i]->GetPosition().y,m_lights[i]->GetPosition().z,m_lights[i]->GetRadius() };
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

Vector3 Renderer::RandomLightPosition(std::uniform_real_distribution<> dis, std::mt19937 gen) {
	Vector3 pos{};
	float temp[3];
	for (int i = 0; i < 3; i++)
	{
		float min = LIGHT_BORDER_MIN[i];
		float max = LIGHT_BORDER_MAX[i];
		GLfloat aa = (GLfloat)dis(gen);
		temp[i] =aa * (max - min) + min;
	}
	pos = Vector3{ temp[0],temp[1],temp[2] };
	return pos;
}



void Renderer::UpdateScene(float dt) {
	m_camera->UpdateCamera(dt);
	viewMatrix = m_camera->BuildViewMatrix();
	m_frustum->FromMatrix(projMatrix*viewMatrix);
	UpdateLights(dt);
	m_fps = CalculateFPS(dt);
}

void Renderer::RenderScene()	{
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	switch (m_shadingType)
	{
	case Renderer::Forward:
		//0.Forward Rendering
		DepthPrePass();
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
		LightCullingPass();
		CalculateLighting();
		break;
	case Renderer::Cluster:
		//3.Cluster Forward Rendering
		DepthPrePass(true);
		ClusterLightCulling();
		ClusterCalculateLighting();
		break;
	case Renderer::ForwardPlus_Debug_Depth:
		DrawDepthDebug();
		break;
	case Renderer::ForwardPlus_Debug_Lights:
		DepthPrePass();
		LightCullingPass();
		DrawLightDebug();
		break;
	default:
		break;
	}

}

void Renderer::ForwardRendering(){
	glBindFramebuffer(GL_FRAMEBUFFER, m_finalHelper->GetFBO());
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	BindShader(m_modelShader);
	UpdateShaderMatrices();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightsSSBO);
	glUniform3fv(glGetUniformLocation(m_modelShader->GetProgram(), "viewPos"), 1, (float*)&m_camera->GetPosition());

	m_model->Draw(m_modelShader);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(m_finalShader);
	glUniform1i(glGetUniformLocation(m_finalShader->GetProgram(), "diffTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_finalHelper->GetTex());
	glUniform1f(glGetUniformLocation(m_finalShader->GetProgram(), "exposure"), m_exposure);
	m_quad->Draw();
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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightsSSBO);

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
		//frustum culling
		if (!m_frustum->InsideFrustum(m_lights[i]->GetPosition(), m_lights[i]->GetRadius()))
		{
			continue;
		}
		//std::cout << i << std::endl;
		glUniform1f(glGetUniformLocation(m_lightingShader->GetProgram(), "lightRadius"), m_lights[i]->GetRadius());
		glUniform3fv(glGetUniformLocation(m_lightingShader->GetProgram(), "lightPos"), 1, (float*)&m_lights[i]->GetPosition());

		glUniform1f(glGetUniformLocation(m_lightingShader->GetProgram(), "pointLight.radius"), m_lights[i]->GetRadius());
		glUniform3fv(glGetUniformLocation(m_lightingShader->GetProgram(), "pointLight.position"), 1, (float*)&m_lights[i]->GetPosition());
		glUniform3fv(glGetUniformLocation(m_lightingShader->GetProgram(), "pointLight.color"), 1, (float*)&m_lights[i]->GetColor());

		m_sphere->Draw();
	}
	//std::cout << "-------------------------" << std::endl;

	//optimize by draw instanced, but performance does not advanced
	//std::vector<Light*> temp;
	//for (int i = 0; i < m_lights.size(); i++)
	//{
	//	//frustum culling
	//	if (m_frustum->InsideFrustum(m_lights[i]->GetPosition(), m_lights[i]->GetRadius()))
	//	{
	//		temp.push_back(m_lights[i]);
	//	}
	//}
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightsSSBO);
	//PointLight* pointLights = (PointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	//for (int i = 0; i < temp.size(); i++)
	//{
	//	PointLight& light = pointLights[i];
	//	light.color = Vector4{ temp[i]->GetColor().x,temp[i]->GetColor().y,temp[i]->GetColor().z,1.0f };
	//	light.position_radius = Vector4{ temp[i]->GetPosition().x, temp[i]->GetPosition().y,temp[i]->GetPosition().z,temp[i]->GetRadius() };
	//}
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//m_sphere->DrawInstanced(temp.size());
	

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}

void Renderer::CombineBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_finalHelper->GetFBO());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(m_finalShader);
	glUniform1i(glGetUniformLocation(m_finalShader->GetProgram(), "diffTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_finalHelper->GetTex());
	glUniform1f(glGetUniformLocation(m_finalShader->GetProgram(), "exposure"), m_exposure);
	m_quad->Draw();
}


void Renderer::DepthPrePass(const bool isCluster){
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthPreHelper->GetFBO());
	glClear(GL_DEPTH_BUFFER_BIT);
	BindShader(m_depthPreShader);
	UpdateShaderMatrices();

	//if (isCluster)
	//{
	//	if (m_frameIndex==0)//when the frame index return to the 0, reset the whole m_activeClusterListSSBO
	//	{
	//		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_activeClusterListSSBO);
	//		int* temp = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	//		memset(temp, 0, m_clusterNumber);
	//		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	//		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//		m_frameIndex++;//finally, reset m_frameIndex to 1
	//	}
	//}

	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_clusterBasicSSBO);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, m_activeClusterListSSBO);

	//glUniform1ui(glGetUniformLocation(m_depthPreShader->GetProgram(), "frameIndex"), m_frameIndex);
	//glUniform1f(glGetUniformLocation(m_depthPreShader->GetProgram(), "zNear"), m_near);
	//glUniform1f(glGetUniformLocation(m_depthPreShader->GetProgram(), "zFar"), m_far);
	//glUniform1i(glGetUniformLocation(m_depthPreShader->GetProgram(), "isCluster"), isCluster);

	m_model->Draw(m_depthPreShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/*glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, 0);*/

	//try to use depth pre pass to decrease over draw
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_depthPreHelper->GetFBO());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_finalHelper->GetFBO());
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::InitForwardPlusRendering() {
	//split screen with 16*16 tiles
	m_workGroupsX = (width + (width % TILE_SIZE)) / TILE_SIZE;//judge if the exceeded pixels greater than half of 16, if it is, will get more tile
	m_workGroupsY = (height + (height % TILE_SIZE)) / TILE_SIZE;
	size_t numberOfTiles = m_workGroupsX * m_workGroupsY;

	glGenBuffers(1, &m_lightsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_LIGHTS * sizeof(PointLight), 0, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_visibleLightIndicesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_visibleLightIndicesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(VisibleIndex) * MAX_LIGHT_NUMBER_PER_CLUSTER, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::LightCullingPass() {
	m_lightCullingShader->Bind();
	glUniformMatrix4fv(glGetUniformLocation(m_lightCullingShader->GetProgram(), "viewMatrix"), 1, false, viewMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(m_lightCullingShader->GetProgram(), "projMatrix"), 1, false, projMatrix.values);

	/*Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(m_lightCullingShader->GetProgram(), "inverseProjView"), 1, false, (float*)& invViewProj);
	Matrix4 invView =  viewMatrix.Inverse();
	glUniformMatrix4fv(glGetUniformLocation(m_lightCullingShader->GetProgram(), "inverseView"), 1, false, (float*)& invView);
*/
	glUniform1i(glGetUniformLocation(m_lightCullingShader->GetProgram(), "depthTex"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_depthPreHelper->GetDepthTex());
	glUniform1i(glGetUniformLocation(m_lightCullingShader->GetProgram(), "lightCount"), NUM_LIGHTS);
	int temp[2]{ width,height };
	glUniform2iv(glGetUniformLocation(m_lightCullingShader->GetProgram(), "screenSize"), 1, &temp[0]);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightsSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_visibleLightIndicesSSBO);

	glDispatchCompute(m_workGroupsX, m_workGroupsY, 1);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Renderer::CalculateLighting() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_finalHelper->GetFBO());

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//if we don't use depth pre pass, we need this statement
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	BindShader(m_fp_lightingShader);
	UpdateShaderMatrices();
	glUniform3fv(glGetUniformLocation(m_fp_lightingShader->GetProgram(), "viewPos"), 1, (float*)&m_camera->GetPosition());
	glUniform1i(glGetUniformLocation(m_fp_lightingShader->GetProgram(), "numberOfTilesX"), m_workGroupsX);

	/*glUniform2f(glGetUniformLocation(m_fp_lightingShader->GetProgram(), "pixelSize"), 1.0f / (float)width, 1.0f / (float)height);
	glUniform1i(glGetUniformLocation(m_fp_lightingShader->GetProgram(), "depthTex"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_depthPreHelper->GetDepthTex());*/

	m_model->Draw(m_fp_lightingShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/*glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, 0);*/


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(m_finalShader);
	glUniform1i(glGetUniformLocation(m_finalShader->GetProgram(), "diffTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_finalHelper->GetTex());
	glUniform1f(glGetUniformLocation(m_finalShader->GetProgram(), "exposure"), m_exposure);
	m_quad->Draw();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	
}

void Renderer::DrawDepthDebug() {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

	BindShader(m_fp_depthDebugShader);
	UpdateShaderMatrices();
	glUniform1f(glGetUniformLocation(m_fp_depthDebugShader->GetProgram(), "near"), 1.0f);
	glUniform1f(glGetUniformLocation(m_fp_depthDebugShader->GetProgram(), "far"), 3000.0f);
	m_model->Draw(m_fp_depthDebugShader);
}

void Renderer::DrawLightDebug() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(m_fp_lightDebugShader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(m_fp_lightDebugShader->GetProgram(), "totalLightCount"), NUM_LIGHTS);
	glUniform1i(glGetUniformLocation(m_fp_lightDebugShader->GetProgram(), "numberOfTilesX"), m_workGroupsX);
	m_model->Draw(m_fp_lightDebugShader);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
}

void Renderer::InitClusterRendering() {
	GLuint sizeX = (GLuint)std::ceilf(width / (float)CLUSTER_SIZE_X);

	//init AABB space of clusters
	glGenBuffers(1, &m_clusterAABBSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_clusterAABBSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_clusterNumber * sizeof(struct ClusterAABB), NULL, GL_STATIC_COPY);

	//set cluster basic data
	m_clusterBaisc.inverseProjection = projMatrix.Inverse();
	m_clusterBaisc.clusterSizes[0] = CLUSTER_SIZE_X;
	m_clusterBaisc.clusterSizes[1] = CLUSTER_SIZE_Y;
	m_clusterBaisc.clusterSizes[2] = CLUSTER_SIZE_Z;
	m_clusterBaisc.clusterSizes[3] = sizeX;
	m_clusterBaisc.screenWidth = width;
	m_clusterBaisc.screenHeight = height;
	m_clusterBaisc.sliceScalingFac = (float)CLUSTER_SIZE_Z / std::log2f(m_far / m_near);
	m_clusterBaisc.sliceBiasFac= -((float)CLUSTER_SIZE_Z * std::log2f(m_near) / std::log2f(m_far / m_near));

	glGenBuffers(1, &m_clusterBasicSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_clusterBasicSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct ClusterBasic), &m_clusterBaisc, GL_STATIC_COPY);

	//init space of light index list
	unsigned int totalNumLights = m_clusterNumber * MAX_LIGHT_NUMBER_PER_CLUSTER;
	glGenBuffers(1, &m_lightIndexListSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightIndexListSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, totalNumLights * sizeof(unsigned int), NULL, GL_STATIC_COPY);

	////init space of light grids
	//glGenBuffers(1, &m_lightGridsSSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightGridsSSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, m_clusterNumber * 2 * sizeof(unsigned int), NULL, GL_STATIC_COPY);

	////init space of light index count
	//glGenBuffers(1, &m_globalLightIndexCountSSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_globalLightIndexCountSSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), NULL, GL_STATIC_COPY);

	////init active cluster list
	//glGenBuffers(1, &m_activeClusterListSSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_activeClusterListSSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, m_clusterNumber  * sizeof(unsigned int), NULL, GL_STATIC_COPY);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//calculate AABB of cluster only once
	m_c_generateClusterShader->Bind();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_clusterAABBSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_clusterBasicSSBO);
	glUniform1f(glGetUniformLocation(m_c_generateClusterShader->GetProgram(), "zNear"), m_near);
	glUniform1f(glGetUniformLocation(m_c_generateClusterShader->GetProgram(), "zFar"), m_far);
	m_c_generateClusterShader->Dispatch(CLUSTER_SIZE_X, CLUSTER_SIZE_Y, CLUSTER_SIZE_Z);
	
}

void Renderer::ClusterLightCulling(){
	m_c_lightCullingShader->Bind();
	glUniform1ui(glGetUniformLocation(m_c_lightCullingShader->GetProgram(), "frameIndex"), m_frameIndex++);
	glUniformMatrix4fv(glGetUniformLocation(m_c_lightCullingShader->GetProgram(), "viewMatrix"), 1, false, viewMatrix.values);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_clusterAABBSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_clusterBasicSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_lightsSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_lightIndexListSSBO);
	/*glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_lightGridsSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_globalLightIndexCountSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, m_activeClusterListSSBO);*/
	//m_c_lightCullingShader->Dispatch(1, 1, 6);
	m_c_lightCullingShader->Dispatch(CLUSTER_SIZE_X, CLUSTER_SIZE_Y, CLUSTER_SIZE_Z);
}


void Renderer::ClusterCalculateLighting() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_finalHelper->GetFBO());
	glClear(GL_COLOR_BUFFER_BIT);//we should retain depth buffer
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	BindShader(m_c_lightingShader);
	UpdateShaderMatrices();

	glUniform3fv(glGetUniformLocation(m_c_lightingShader->GetProgram(), "viewPos"), 1, (float*)&m_camera->GetPosition());
	glUniform1f(glGetUniformLocation(m_c_lightingShader->GetProgram(), "zNear"), m_near);
	glUniform1f(glGetUniformLocation(m_c_lightingShader->GetProgram(), "zFar"), m_far);
	glUniform1i(glGetUniformLocation(m_c_lightingShader->GetProgram(), "showSlice"), m_showSlices);
	m_model->Draw(m_c_lightingShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, 0);
	/*glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, 0);*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(m_finalShader);
	glUniform1i(glGetUniformLocation(m_finalShader->GetProgram(), "diffTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_finalHelper->GetTex());
	glUniform1f(glGetUniformLocation(m_finalShader->GetProgram(), "exposure"), m_exposure);
	m_quad->Draw();
}