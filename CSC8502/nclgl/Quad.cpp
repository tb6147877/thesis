#include "Quad.h"


Quad::Quad() {
    float planeVertices[] = {
        // positions          // texture Coords 
         -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
          1.0f, 1.0f, 0.0f,  1.0f, 1.0f,

         -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f, 1.0f, 0.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}

Quad::~Quad(){}

void Quad::Draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6); 
    glBindVertexArray(0);
}