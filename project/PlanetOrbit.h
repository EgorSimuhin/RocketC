#pragma once
#include "include/glad/include/glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class PlanetOrbit
{
public:
    void DrawOrbit(const std::vector<glm::vec3>& orbitPath)
    {
        // генерация VAO, VBO для рисования линий
        unsigned int VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, orbitPath.size() * sizeof(glm::vec3), orbitPath.data(), GL_STATIC_DRAW);

        // настройка вершинных атрибутов
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // рисование линий
        glDrawArrays(GL_LINE_LOOP, 0, orbitPath.size());

        // очистка ресурсов
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}; 
