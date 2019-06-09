#include "GameMove.h"

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

}

// 控制收枪（右键关镜）
void GameMove::putDownGun() {

}

// 枪械动作总控制
void GameMove::gunMove() {
	if (this->isGunRaiseUp) {
		this->putDownGun();
	}
	else {
		this->raiseUpGun();
	}
	this->isGunRaiseUp = !this->isGunRaiseUp;
}