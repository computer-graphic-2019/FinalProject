#include "GameMove.h"

extern unsigned int SCR_WIDTH, SCR_HEIGHT;
extern ResourceManager ResM;
extern GameMove moveController;

GameMove::GameMove() {
	this->humanCamera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));
	this->cameraSpeed = 1.0f;
}

// 控制人物前移(w)
void GameMove::humanMoveForward() {
	this->humanCamera->ProcessKeyboard(UP, this->cameraSpeed);
}

// 控制人物后退(s)
void GameMove::humanMoveBackward() {
	this->humanCamera->ProcessKeyboard(DOWN, this->cameraSpeed);
}

// 控制人物左转(a)
void GameMove::humanMoveLeft() {
	this->humanCamera->ProcessKeyboard(LEFT, this->cameraSpeed);
}

// 控制人物右转(d)
void GameMove::humanMoveRight() {
	this->humanCamera->ProcessKeyboard(RIGHT, this->cameraSpeed);
}

// 控制人物旋转
void GameMove::humanRotate(float xoffset, float yoffset) {
	this->humanCamera->ProcessMouseMove(xoffset, yoffset);
}

// 人物移动总控制
void GameMove::humanMove(GLFWwindow *window, float deltaTime) {
	this->cameraSpeed = deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		humanMoveForward();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		humanMoveBackward();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		humanMoveLeft();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		humanMoveRight();
	}	
}

Camera* GameMove::getHumanCamera() const {
	return this->humanCamera;
}

// 控制举枪（右键开镜）
void GameMove::raiseUpGun() {
	// 场景放大
	moveController.getHumanCamera()->setZoom(30.0f);
	// 绘制枪
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.85f, -0.66f));
	model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ResM.getShader("model")->use();
	ResM.getShader("model")->setMat4("view", glm::mat4(1.0f));
	ResM.getShader("model")->setMat4("model", model);
	ResM.getShader("model")->setMat4("projection", projection);
	ResM.getModel("gunOnFire")->Draw((*ResM.getShader("model")));
}

// 控制收枪（右键关镜）
void GameMove::putDownGun() {
	// 场景缩小
	moveController.getHumanCamera()->setZoom(45.0f);
	// 绘制枪
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.1f, -0.1f, -0.5f));
	model = glm::scale(model, glm::vec3(0.013f, 0.013f, 0.013f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 gunRotate = glm::rotate(glm::mat4(1.0f), glm::radians(moveController.getHumanCamera()->getPitch()), glm::vec3(1.0f, 0.0f, 0.0f));
	gunRotate = glm::rotate(gunRotate, glm::radians(moveController.getHumanCamera()->getYaw()), glm::vec3(0.0f, 1.0f, 0.0f));
	
	ResM.getShader("model")->use();
	ResM.getShader("model")->setMat4("gunRotate", gunRotate);
	ResM.getShader("model")->setMat4("view", glm::mat4(1.0f));
	ResM.getShader("model")->setMat4("model", model);
	ResM.getShader("model")->setMat4("projection", projection);
	ResM.getModel("gun")->Draw((*ResM.getShader("model")));
}

// 枪械动作总控制
void GameMove::gunMove(bool signal) {
	this->isGunRaiseUp = signal;
	if (this->isGunRaiseUp) {
		this->raiseUpGun(); 
	}
	else {
		this->putDownGun();
	}
}