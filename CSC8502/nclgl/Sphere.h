#pragma once

#include "glad\glad.h"
#include <vector>

#include "Vector2.h"
#include "Vector3.h"

class Sphere
{
public:
	Sphere();
	~Sphere();
	void Draw();
private:
	int indexCount;
	GLuint VAO, VBO, EBO;
};

