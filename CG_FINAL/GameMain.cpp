#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Camera.h"
#include "GameResource.h"
#include "GameMove.h"
#include "SkyBox.h"
#include "GlobalVar.h"

#include <iostream>



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
    //Shader ourShader("./1.model_loading.vs", "./1.model_loading.fs");
	ResM.loadShader("model", "./ShaderCode/1.model_loading.vs", "./ShaderCode/1.model_loading.fs");
    // load models
    // -----------
    //Model ourModel("./models/gun/gun_update.obj");
	ResM.loadModel("place", "./models/place/scene.obj");
	ResM.loadModel("target", "./models/target/target.obj");
	ResM.loadModel("gun", "./models/gun/m24.obj");

    
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
		skybox.renderSkyBox(glm::mat4(glm::mat3(moveController.getHumanCamera()->getView())), glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f));
		
        // don't forget to enable shader before setting uniforms
        //ourShader.use();
		ResM.getShader("model")->use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f); 
		//glm::mat4 view = camera.getView();
		glm::mat4 view = moveController.getHumanCamera()->getView();
		ResM.getShader("model")->setMat4("projection", projection);
		ResM.getShader("model")->setMat4("view", view);

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

		// »æÖÆÇ¹
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.1f, -0.1f, -0.5f));
		model = glm::scale(model, glm::vec3(0.013f, 0.013f, 0.013f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ResM.getShader("model")->setMat4("view", glm::mat4(1.0f));
		ResM.getShader("model")->setMat4("model", model);
		ResM.getShader("model")->setMat4("projection", projection);
		ResM.getModel("gun")->Draw((*ResM.getShader("model")));
		

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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

	/*
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
	*/
	moveController.humanMove(window, deltaTime);
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

	// camera view move horizon 
    //camera.ProcessMouseMove(xoffset, 0);
	moveController.humanRotate(xoffset, 0);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
