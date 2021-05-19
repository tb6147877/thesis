#pragma once

#include <glad/glad.h> 



#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "Vector3.h"
#include "Shader.h"
#include "Assimp_Mesh.h"

class Assimp_Model
{
public:
	Assimp_Model(const std::string& path);
	void Draw(Shader* shader);

private:
	std::vector<Texture> m_textures_loaded;
	std::vector<Assimp_Mesh> m_meshes;
	std::string m_directory;

	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Assimp_Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

