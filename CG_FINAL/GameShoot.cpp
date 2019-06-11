#include "GameShoot.h"

#include "GameMove.h"

extern ResourceManager ResM;
extern GameMove moveController;
extern bool firstTimeShowBullet;
extern unsigned int SCR_WIDTH, SCR_HEIGHT;

// 控制开枪（鼠标左键）
void GameShoot::Fire() {
	firstTimeShowBullet = true;
	std::cout << "Pos: " << bulletPos.x << " " << bulletPos.y << " " << bulletPos.z << std::endl;
	std::cout << "Dir: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
}

void GameShoot::showBullet(float deltaTime) {
	if (firstTimeShowBullet) {
		bulletPos = moveController.getHumanCamera()->getPosition();
		direction = moveController.getHumanCamera()->getFrontVec();
		firstTimeShowBullet = false;
	}
	else {
		bulletPos = bulletPos + 0.01f * direction;
	}
	//std::cout << "Pos: " << pos.x << " " << pos.y << " " << pos.z << std::endl;
	// 绘制枪
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, bulletPos);
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	ResM.getShader("model")->use();
	ResM.getShader("model")->setMat4("view", moveController.getHumanCamera()->getView());
	ResM.getShader("model")->setMat4("model", model);
	ResM.getShader("model")->setMat4("projection", projection);
	ResM.getModel("bullet")->Draw((*ResM.getShader("model")));
}

// 检查命中位置
void GameShoot::CheckCollisionWithTarget() {
	
};

// 计算命中得分
void GameShoot::CalculateScore() {

};

// 计算是否需要装弹（非必需）
void GameShoot::CheckIfNeedReload() {

};