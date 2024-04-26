#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class LineRenderer {
public:
    GLuint VAO, VBO;
    std::vector<glm::vec3> points;

    LineRenderer(std::vector<glm::vec3> points) : points(points) {
        // Генерация буферов и массивов
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // Привязка массива вершин и данных вершин
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);

        // Устанавливаем атрибуты вершин
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    // Отрисовка линии
    void Draw(Shader& shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glBindVertexArray(VAO);
	glLineWidth(0.2f);
        glDrawArrays(GL_LINE_STRIP, 0, points.size());
    }
};
