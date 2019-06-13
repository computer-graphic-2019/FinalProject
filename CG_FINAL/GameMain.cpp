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

#include <iostream>
#include <cmath>

// global value
extern unsigned int SCR_WIDTH, SCR_HEIGHT;
extern ResourceManager ResM;
extern GameMove moveController;
extern GameShoot shootController;
extern float deltaTime, lastFrame;
extern bool gunRaiseUp;

extern std::map<std::string, GameObject> targetList;
extern std::map<std::string, GameObject> movingTargetList;
extern std::map<std::string, GameObject> explodeTargeList;

// global function
extern void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern void mouse_callback(GLFWwindow* window, double xpos, double ypos);
extern void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
extern void processInput(GLFWwindow *window);
extern void processMouseClick(GLFWwindow* window, int button, int action, int mods);

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

	// skybox module
	SkyBox skybox;

    // build and compile shaders
    // -------------------------
	//ResM.loadShader("model", "./ShaderCode/1.model_loading.vs", "./ShaderCode/1.model_loading.fs");
	ResM.loadShader("model", "./ShaderCode/3.phong_shading.vs", "./ShaderCode/3.phong_shading.fs");
    // load models
    // -----------
	ResM.loadModel("place", "./models/place/scene.obj");
	ResM.loadModel("target", "./models/target/target.obj");
	ResM.loadModel("gun", "./models/gun/m24.obj");
	ResM.loadModel("gunOnFire", "./models/gun/m24OnFire.obj");
	ResM.loadModel("bullet", "./models/bullet/bullet.obj");

	// target position
	targetList.insert(std::pair<std::string, GameObject>("target", GameObject(glm::vec3(-0.3f, 0.1f, 20.0f), glm::vec3(0.8f,1.0f,2.0f))));

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render skybox
		skybox.renderSkyBox(glm::mat4(glm::mat3(moveController.getHumanCamera()->getView())), glm::perspective(glm::radians(moveController.getHumanCamera()->getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f));
		
        // don't forget to enable shader before setting uniforms
		ResM.getShader("model")->use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(moveController.getHumanCamera()->getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = moveController.getHumanCamera()->getView();
		glm::vec3 viewPos = moveController.getHumanCamera()->getPosition();

		ResM.getShader("model")->setMat4("gunRotate", glm::mat4(1.0f));
		ResM.getShader("model")->setMat4("projection", projection);
		ResM.getShader("model")->setMat4("view", view);
		ResM.getShader("model")->setVec3("viewPos", viewPos);
		ResM.getShader("model")->setVec3("lightDirection", cos(glfwGetTime()), -0.5f, sin(glfwGetTime()));

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ResM.getShader("model")->setMat4("model", model);
		ResM.getModel("place")->Draw((*ResM.getShader("model")));

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 100.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ResM.getShader("model")->setMat4("model", model);
		ResM.getModel("target")->Draw((*ResM.getShader("model")));
	
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
