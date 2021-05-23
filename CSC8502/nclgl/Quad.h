#pragma once

#include "glad\glad.h"

class Quad
{
public:
	Quad();
	~Quad();

	void Draw();
private:
	GLuint VAO, VBO;

};

