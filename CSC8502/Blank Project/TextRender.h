#pragma once

#include "glad\glad.h"

#include <string>
#include <iostream>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../nclgl/Vector2.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Matrix4.h"

class TextRender
{
public:
	struct Character {
		unsigned int TextureID; // ID handle of the glyph texture
		Vector2   Size;      // Size of glyph
		Vector2   Bearing;   // Offset from baseline to left/top of glyph
		unsigned int Advance;   // Horizontal offset to advance to next glyph
	};

	TextRender(const int width, const int height);
	~TextRender();

	void RenderText(std::string text, float x, float y, float scale, Vector3 color);
private:
	GLuint VAO, VBO;
	std::map<GLchar, Character> Characters;
	Shader* m_shader;
	Matrix4 m_projection;
	FT_Library ft;
	FT_Face face;

};

