#include "Assimp_Model.h"

Assimp_Model::Assimp_Model(const std::string& path) {
	LoadModel(MODELSDIR + path);
}

void Assimp_Model::Draw(Shader* shader) {
	for (int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Draw(shader);
	}
}

void Assimp_Model::LoadModel(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
		return;
	}
	m_directory = path.substr(0, path.find_last_of('/'));
	ProcessNode(scene->mRootNode, scene);
}


void Assimp_Model::ProcessNode(aiNode* node, const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Assimp_Mesh Assimp_Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		Vector3 v3;

		//position
		v3.x = mesh->mVertices[i].x;
		v3.y = mesh->mVertices[i].y;
		v3.z = mesh->mVertices[i].z;
		vertex.Position = v3;
		// normals
		if (mesh->HasNormals())
		{
			v3.x = mesh->mNormals[i].x;
			v3.y = mesh->mNormals[i].y;
			v3.z = mesh->mNormals[i].z;
			vertex.Normal = v3;
		}

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			Vector2 v2;
			// texture coordinates
			v2.x = mesh->mTextureCoords[0][i].x;
			v2.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = v2;
			// tangent
			v3.x = mesh->mTangents[i].x;
			v3.y = mesh->mTangents[i].y;
			v3.z = mesh->mTangents[i].z;
			vertex.Tangent = v3;
			// bitangent
			v3.x = mesh->mBitangents[i].x;
			v3.y = mesh->mBitangents[i].y;
			v3.z = mesh->mBitangents[i].z;
			vertex.Bitangent = v3;
		}
		else
			vertex.TexCoords = Vector2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	// indices
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// 1. diffuse maps
	vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	return Assimp_Mesh(vertices, indices, textures);
}

std::vector<Texture> Assimp_Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		
		bool skip = false;
		for (int j = 0; j < m_textures_loaded.size(); j++)
		{
			if (std::strcmp(m_textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(m_textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id =Texture::TextureFromFile(str.C_Str(), this->m_directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			m_textures_loaded.push_back(texture);
		}
	}
	return textures;
}