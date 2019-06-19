#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Camera.h"
#include "GameResource.h"
#include "GameMove.h"
#include "SkyBox.h"
#include "GameShoot.h"
#include "GameTools.h"

#include <iostream>
#include <cmath>
#include <deque>

// global value
extern unsigned int SCR_WIDTH, SCR_HEIGHT;
extern ResourceManager ResM;
extern GameMove moveController;
extern GameShoot shootController;
extern PhysicsEngine physicsEngine;
extern float deltaTime, lastFrame;
extern bool gunRaiseUp;

extern std::map<std::string, GameObject> targetList;
extern std::map<std::string, GameObject> movingTargetList;
extern std::map<std::string, GameObject> explodeTargeList;
extern std::map<std::string, bool> explodeTargeRec;
extern std::deque<std::string> recoverList;

// global function
extern void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern void mouse_callback(GLFWwindow* window, double xpos, double ypos);
extern void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
extern void processInput(GLFWwindow *window);
extern void processMouseClick(GLFWwindow* window, int button, int action, int mods);

float reset_interval = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
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
	glfwSetMouseButtonCallback(window, processMouseClick);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_FRAMEBUFFER_SRGB);

	// skybox module
	SkyBox skybox;
	// render director
	GameTools director(glm::vec3(1.0f), 0.2, 0.5, 0.3);


    // build and compile shaders
    // -------------------------
	ResM.loadShader("textShader", "./ShaderCode/5.text_loading.vs", "./ShaderCode/5.text_loading.fs");
    // load models
	ResM.loadModel("gun", "./models/gun/m24.obj");
	ResM.loadModel("gunOnFire", "./models/gun/m24OnFire.obj");
	ResM.loadModel("bullet", "./models/bullet/bullet.obj");

	// target position
	targetList.insert(std::pair<std::string, GameObject>("target", GameObject(glm::vec3(-0.3f, 0.1f, -20.0f), glm::vec3(0.8f,1.0f,2.0f))));
	explodeTargeList.insert(std::pair<std::string, GameObject>("explodeTarget1", GameObject(glm::vec3(1.2f, 0.2f, -20.0f), glm::vec3(0.4f, 0.7f, 1.0f))));
	explodeTargeRec.insert(std::pair<std::string, bool>("explodeTarget1", false));

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shootController.initialTextShader();

	// set boundary
	// ���õذ���ײ��
	physicsEngine.setSceneInnerBoundary(glm::vec3(-200.0f, 0.0f, -200.0f), glm::vec3(200.0f, 0.0f, 200.0f));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

		// reset environment
		reset_interval += deltaTime;
		if (reset_interval > 5.0f) {
			if (!recoverList.empty()) {
				explodeTargeRec[recoverList.front()] = false;
				recoverList.pop_front();
			}
			reset_interval = 0.0f;
		}

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// initialize 
		float time = currentFrame / 10;
		glm::vec3 lightPos(100 * cos(time), 100 * sin(time), 1.0);

		// render skybox
		skybox.renderSkyBox(time);
		// render scene
		director.RenderDepthMap(lightPos);
		director.RenderScene(lightPos);
		//director.testMap(window);
		
		// raise up gun
		moveController.gunMove(gunRaiseUp);

		// show bullet
		shootController.showBullet(deltaTime);
		shootController.CheckCollisionWithTarget();
		
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
