#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "model.h"
#include <iostream>
#include "EphemerisRelease.hpp"
#include "RK.h"
#include "PlanetOrbit.h"
#include <memory>
#include "LineRenderer.h"

const std::array<std::string, 12> key = {
                                        "MERCURY",
                                        "VENUS",
                                        "EARTH",
                                        "MARS",
                                        "JUPYTER",
                                        "SATURN",
                                        "URANUS",
                                        "NEPTUNE",
                                        "PLUTO",
                                        "MOON",
                                        "SUN",
                                        "ROCKET"
                                         };

std::vector<glm::vec3> perevod_planets(
					    const int k,
					    const State_full full
					    )
{
	std::vector<glm::vec3> result;
	for (auto i : full.planets_solution)
	{
		glm::vec3 temp;
		temp.z = i[k][0]/10e9;
		temp.x = i[k][1]/10e9;
		temp.y = i[k][2]/10e9;
		result.push_back(temp);
	}
	return result;
}

std::vector<glm::vec3> perevod_rocket(const State_full full)
{
	std::vector<glm::vec3> result;
	for (auto i : full.rocket_solution)
	{
		glm::vec3 temp;
                temp.x = i.cond[0]/10e9;
                temp.y = i.cond[1]/10e9;
                temp.z = i.cond[2]/10e9;
                result.push_back(temp);
	}
	return result;
}

std::vector<double> perevod_time(
				const State_full full
				)
{
	std::vector<double> result;
	for (auto i : full.rocket_solution)
	{
		result.push_back(i.time);
	}
	return result;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    std::string filePath("lnxp1600p2200.405");
	dph::EphemerisRelease de405(filePath);
        double JED, step, endTime, x_0, y_0, z_0, v_x0, v_y0, v_z0;
	
	std::cout << "Введите дату в юлианских днях (ссылка на калькулятор: https://planetcalc.ru/503/)" << std::endl;
	std::cin >> JED;
	std::cout << "Введите начальные условия (x_0, y_0, z_0, v_x0, v_y0, v_z0) в метрах" << std::endl;
	std::cin >> x_0 >> y_0 >> z_0 >> v_x0 >> v_y0 >> v_z0;
	std::cout << "Введите время шага интегрирования в секундах" << std::endl;
	std::cin >> step;
	std::cout << "Введите длительность времени расчета в секундах" << std::endl;
	std::cin >> endTime;

	const Eigen::Vector<double, 6> cond_0 = {x_0, y_0, z_0, v_x0, v_y0, v_z0};
	const double initialTime = 0;
   
	const State initialState = {cond_0, initialTime};

 
	const State_full full = rungeKutta(initialState, step, endTime, JED, de405);

	std::map<std::string, std::vector<glm::vec3>> myMap;

	for (int i = 0; i < 11; ++i)
	{
		myMap.emplace(key[i], perevod_planets(i, full));
    	}
   	myMap.emplace(key.back(), perevod_rocket(full));

	const std::vector<double> time = perevod_time(full);		
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");

    Model venus("/home/maratkhab/activeproj/project/modeles/Venus_v1_L3.123c5f86bd5e-26c0-4e50-bae1-911256cb7689/13901_Venus_v1_l3.obj", 0.002f);
    Model sun("/home/maratkhab/activeproj/project/modeles/Venus_v1_L3.123c5f86bd5e-26c0-4e50-bae1-911256cb7689/13901_Venus_v1_l3.obj", 0.008f);
    Model mars("/home/maratkhab/activeproj/project/modeles/Mars_v1_L3.123c794d6114-bc98-4e8c-8486-493396506fb0/13903_Mars_v1_l3.obj", 0.001f);
    Model saturn("/home/maratkhab/activeproj/project/modeles/Saturn_v1_L3.123ca3750520-9e3a-478a-8d9b-97e3a212a44c/13906_Saturn_v1_l3.obj", 0.005f);
    Model earth("/home/maratkhab/activeproj/project/modeles/Earth_v1_L3.123cce489830-ca89-49f4-bb2a-c921cce7adb2/13902_Earth_v1_l3.obj", 0.002f);
    Model moon("/home/maratkhab/activeproj/project/modeles/Moon_3D_Model/moon.obj", 0.001f);
    Model jupyter("/home/maratkhab/activeproj/project/modeles/Jupiter_v1_L3.123c7d3fa769-8754-46f9-8dde-2a1db30a7c4e/13905_Jupiter_V1_l3.obj", 0.004f);
    Model mercury("/home/maratkhab/activeproj/project/modeles/Mercury_v1_L3.123cc7069d02-51a1-4f1a-a9ac-3b10e52568dc/13900_Mercury_v1_l3.obj", 0.001f);
    Model uranus("/home/maratkhab/activeproj/project/modeles/Mercury_v1_L3.123cc7069d02-51a1-4f1a-a9ac-3b10e52568dc/13900_Mercury_v1_l3.obj", 0.002f);
    Model neptun("/home/maratkhab/activeproj/project/modeles/Neptune_v2_L3.123c6fe2b903-2de3-4b54-836a-dd427a10e972/13908_Neptune_V2_l3.obj", 0.002f);
    Model pluto("/home/maratkhab/activeproj/project/modeles/Pluto_v1_L3.123c3daa7fdb-a7d7-4bd1-b360-fd69551814cb/13909_Pluto_v1_l3.obj ", 0.0001f);
    Model rocket("/home/maratkhab/activeproj/project/modeles/UFO_Saucer_v1_L2.123c50bd261a-1751-44c1-b973-f0dd9e11cecd/13884_UFO_Saucer_v1_l2.obj", 0.001f);
    Shader shader("shader.vs", "shader.fs");
    uint32_t i = 0;
    std::map<std::string, std::shared_ptr<LineRenderer>> lineRenderers;
    for (const auto& planet : myMap) {
        std::vector<glm::vec3> linePoints = planet.second;
        std::shared_ptr<LineRenderer> lineRenderer = std::make_shared<LineRenderer>(linePoints);
        lineRenderers[planet.first] = lineRenderer;
    }
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        for (const auto& renderer : lineRenderers) {
            glm::mat4 model = glm::mat4(1.0f);
            renderer.second->Draw(ourShader, model, view, projection);
        }

        ourShader.use();

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);


        if (i > myMap["EARTH"].size()) {i = 0;}
        //line.DrawLine(myMap["EARTH"]);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["EARTH"][i]);
	std::cout<<myMap["EARTH"][i].x<<std::endl;
        earth.Draw(ourShader, model);

	ourShader.use();
	model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["VENUS"][i]);
        std::cout<<myMap["VENUS"][i].x<<std::endl;
        venus.Draw(ourShader, model);

	ourShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["MARS"][i]);
        std::cout<<myMap["MARS"][i].x<<std::endl;
        mars.Draw(ourShader, model);

	ourShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["MERCURY"][i]);
        std::cout<<myMap["MERCURY"][i].x<<std::endl;
        mercury.Draw(ourShader, model);

        ourShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["JUPYTER"][i]);
        std::cout<<myMap["JUPYTER"][i].x<<std::endl;
        jupyter.Draw(ourShader, model);

        ourShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["SATURN"][i]);
        std::cout<<myMap["SATURN"][i].x<<std::endl;
        saturn.Draw(ourShader, model);

        ourShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["URANUS"][i]);
        std::cout<<myMap["URANUS"][i].x<<std::endl;
        uranus.Draw(ourShader, model);

        ourShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["NEPTUNE"][i]);
        std::cout<<myMap["NEPTUNE"][i].x<<std::endl;
        neptun.Draw(ourShader, model);

        ourShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["PLUTO"][i]);
        std::cout<<myMap["PLUTO"][i].x<<std::endl;
        venus.Draw(ourShader, model);

        ourShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["MOON"][i]);
        std::cout<<myMap["MOON"][i].x<<std::endl;
        moon.Draw(ourShader, model);

        ourShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["SUN"][i]);
        std::cout<<myMap["SUN"][i].x<<std::endl;
        sun.Draw(ourShader, model);

        ourShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, myMap["ROCKET"][i]);
        std::cout<<myMap["ROCKET"][i].x<<std::endl;
        rocket.Draw(ourShader, model);
        glfwSwapBuffers(window);
        glfwPollEvents();
	i++;
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ResetCameraPosition();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	camera.ProcessVerticalMovement(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessVerticalMovement(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
