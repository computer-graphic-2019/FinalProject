#ifndef GAME_SHOOT_H
#define GAME_SHOOT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Camera.h"
#include "GameResource.h"
#include "GameObject.h"
#include "GameMove.h"

#include <iostream>

class GameShoot {
private:
	glm::vec3 bulletPos;
	glm::vec3 direction;
	GameObject bullet;
	bool isHit;
public:
	GameShoot() {
		this->bulletPos = glm::vec3(0.0f, 0.0f, -200.0f);
		this->isHit = false;
	}
    // 控制开枪（鼠标左键）
    void Fire();
	// 子弹轨迹
	void showBullet(float deltaTime);
    // 检查命中位置
    void CheckCollisionWithTarget();
    // 计算命中得分
    void CalculateScore();
    // 计算是否需要装弹（非必需）
    void CheckIfNeedReload();
};

#endif