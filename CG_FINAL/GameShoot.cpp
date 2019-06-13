#include "GameShoot.h"

extern ResourceManager ResM;
extern GameMove moveController;
extern bool firstTimeShowBullet;
extern unsigned int SCR_WIDTH, SCR_HEIGHT;
extern bool gunRaiseUp;

extern std::map<std::string, GameObject> targetList;
extern std::map<std::string, GameObject> movingTargetList;
extern std::map<std::string, GameObject> explodeTargeList;
extern std::map<std::string, bool> explodeTargeRec;

// 控制开枪（鼠标左键）
void GameShoot::Fire() {
	// 开镜后才能开枪
	if (gunRaiseUp) {
		firstTimeShowBullet = true;
		this->isHit = false;
	}
}

void GameShoot::showBullet(float deltaTime) {
	if (firstTimeShowBullet) {
		bulletPos = moveController.getHumanCamera()->getPosition();
		direction = moveController.getHumanCamera()->getFrontVec();
		firstTimeShowBullet = false;
	}
	else {
		bulletPos = bulletPos + 0.1f * direction;
	}
	// 子弹轨迹
	this->bullet.Position = bulletPos;

	if (!this->isHit) {
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
}

// 检查命中位置
void GameShoot::CheckCollisionWithTarget() {
	// 固定靶子
	if (!this->isHit) {
		for (std::map<std::string, GameObject>::iterator ptr = targetList.begin(); ptr != targetList.end(); ptr++) {
			//std::cout << "target: " << ptr->second.Position.x << " " << ptr->second.Position.y << " " << ptr->second.Position.z << std::endl;
			//std::cout << "bullet: " << this->bullet.Position.x << " " << this->bullet.Position.y << " " << this->bullet.Position.z << std::endl;
			if (ptr->second.CheckCollision(this->bullet)) {
				this->isHit = true;
			}
		}
	}
	// 移动靶子
	if (!this->isHit) {
		for (std::map<std::string, GameObject>::iterator ptr = movingTargetList.begin(); ptr != movingTargetList.end(); ptr++) {
			//std::cout << "target: " << ptr->second.Position.x << " " << ptr->second.Position.y << " " << ptr->second.Position.z << std::endl;
			//std::cout << "bullet: " << this->bullet.Position.x << " " << this->bullet.Position.y << " " << this->bullet.Position.z << std::endl;
			if (ptr->second.CheckCollision(this->bullet)) {
				this->isHit = true;
			}
		}
	}
	// 有爆炸效果的靶子
	if (!this->isHit) {
		for (std::map<std::string, GameObject>::iterator ptr = explodeTargeList.begin(); ptr != explodeTargeList.end(); ptr++) {
			//std::cout << "target: " << ptr->second.Position.x << " " << ptr->second.Position.y << " " << ptr->second.Position.z << std::endl;
			//std::cout << "bullet: " << this->bullet.Position.x << " " << this->bullet.Position.y << " " << this->bullet.Position.z << std::endl;
			if (ptr->second.CheckCollision(this->bullet)) {
				this->isHit = true;
				explodeTargeRec[ptr->first] = true;
			}
		}
	}
};

// 计算命中得分
void GameShoot::CalculateScore() {

};

// 计算是否需要装弹（非必需）
void GameShoot::CheckIfNeedReload() {

};