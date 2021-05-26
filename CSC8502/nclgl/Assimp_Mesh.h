#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include <string>
#include <vector>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Shader.h"

struct Vertex {
    // position
    Vector3 Position;
    // normal
    Vector3 Normal;
    // texCoords
    Vector2 TexCoords;
    // tangent
    Vector3 Tangent;
    // bitangent
    Vector3 Bitangent;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;

    static unsigned int TextureFromFile(const char* path, const std::string& directory, const std::string& type) {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3) {
                format = GL_RGB;
                /*if ("texture_diffuse"==type)
                {
                    format = GL_SRGB;
                }*/
            }
            else if (nrComponents == 4) {
                format = GL_RGBA;
                /*if ("texture_diffuse" == type)
                {
                    format = GL_SRGB_ALPHA;
                }*/
            }
                

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
};
class Assimp_Mesh
{
public:
    Assimp_Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);
    void Draw(Shader* shader);

private:
    vector<Vertex>       m_vertices;
    vector<unsigned int> m_indices;
    vector<Texture>      m_textures;
    unsigned int m_VAO, m_VBO, m_EBO;

    void SetupMesh();
};

