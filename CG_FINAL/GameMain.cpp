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
extern std::map<std::string, bool> explodeTargeRec;

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
	ResM.loadShader("depth", "./ShaderCode/5.depth.vs", "./ShaderCode/5.depth.fs");
	ResM.loadShader("explodeModel", "./ShaderCode/4.explode_shading.vs", "./ShaderCode/4.explode_shading.fs", "./ShaderCode/4.explode_shading.gs");
    // load models
    // -----------
	ResM.loadModel("place", "./models/place/scene.obj");
	ResM.loadModel("target", "./models/target/target.obj");
	ResM.loadModel("gun", "./models/gun/m24.obj");
	ResM.loadModel("gunOnFire", "./models/gun/m24OnFire.obj");
	ResM.loadModel("bullet", "./models/bullet/bullet.obj");
	ResM.loadModel("explodeTarget", "./models/explodeTarget/explodeTarget.obj");

	// target position
	targetList.insert(std::pair<std::string, GameObject>("target", GameObject(glm::vec3(-0.3f, 0.1f, 20.0f), glm::vec3(0.8f,1.0f,2.0f))));
	explodeTargeList.insert(std::pair<std::string, GameObject>("explodeTarget1", GameObject(glm::vec3(1.5f, 0.1f, 20.0f), glm::vec3(0.8f, 1.0f, 2.0f))));
	explodeTargeRec.insert(std::pair<std::string, bool>("explodeTarget1", false));

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 初始化阴影贴图（待修改）
	unsigned int depthMapFBO, depthMap;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		100, 100, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
		// skybox.renderSkyBox(glm::mat4(glm::mat3(moveController.getHumanCamera()->getView())), glm::perspective(glm::radians(moveController.getHumanCamera()->getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f));
		
		// 获取阴影贴图（待修改）
		ResM.getShader("depth")->use();
		glm::vec3 lightPos = glm::vec3(0, 5.0f, 20 + 10 * sin(currentFrame));
		glm::mat4 lightView = glm::mat4(1.0f);
		glm::mat4 lightProjection = glm::mat4(1.0f);
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
		float near_plane = 1.0f, far_plane = 100.0f;
		lightProjection = glm::perspective(glm::radians(120.0f), 1.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		ResM.getShader("depth")->setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, 1000, 1000);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		// glCullFace(GL_FRONT);
		 // render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ResM.getShader("depth")->setMat4("model", model);
		ResM.getModel("place")->Draw((*ResM.getShader("depth")));

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 100.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ResM.getShader("depth")->setMat4("model", model);
		ResM.getModel("target")->Draw((*ResM.getShader("depth")));
		// glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// 渲染光照和阴影（待修改）
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
		ResM.getShader("model")->setVec3("lightDirection", lightPos);
		ResM.getShader("model")->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glBindTexture(GL_TEXTURE_2D, depthMap);

        // render the loaded model
        model = glm::mat4(1.0f);
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

		// explode target
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(7.0f, 3.0f, 100.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		for (std::map<std::string, bool>::iterator ptr = explodeTargeRec.begin(); ptr != explodeTargeRec.end(); ptr++) {
			if (ptr->second) {
				ResM.getShader("explodeModel")->use();
				ResM.getShader("explodeModel")->setMat4("gunRotate", glm::mat4(1.0f));
				ResM.getShader("explodeModel")->setMat4("projection", projection);
				ResM.getShader("explodeModel")->setMat4("view", view);
				ResM.getShader("explodeModel")->setVec3("viewPos", viewPos);
				ResM.getShader("explodeModel")->setVec3("lightDirection", cos(glfwGetTime()), -0.5f, sin(glfwGetTime()));
				// add time component to geometry shader in the form of a uniform
				ResM.getShader("explodeModel")->setFloat("time", glfwGetTime()); //爆炸效果改这里
				ResM.getShader("explodeModel")->setMat4("model", model);
				ResM.getModel("explodeTarget")->Draw((*ResM.getShader("explodeModel")));
			}
			else {
				ResM.getShader("model")->setMat4("model", model);
				ResM.getModel("explodeTarget")->Draw((*ResM.getShader("model")));
			}
		}
	
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
