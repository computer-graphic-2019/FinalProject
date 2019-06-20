#ifndef GAMEPARTICLE_H
#define GAMEPARTICLE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glew.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Particle.h"
#include "GameResource.h"

extern ResourceManager ResM;

class GameParticle {
public:
	GameParticle() {
		ResM.loadTexture("snow", "");
		particleObj = new CParticle();
	}

	~GameParticle() {
		delete particleObj;
	}

	bool InitParticle() {
		return InitSnow();
	}

	void UpdateParticle() {
		UpdateSnow();
	}

	bool InitSnow() {
		for (int i = 0; i < this->particleObj->GetNumOfParticle(); ++i)
		{
			///初始化颜色（白色）
			r = 255;
			g = 255;
			b = 255;
			this->particleObj->SetColor(i, r, g, b);

			///初始化坐标
			x = 0.1f * (rand() % 50) - 2.5f;
			y = 2 + 0.1f * (rand() % 2);
			if ((int)x % 2 == 0)
				z = rand() % 6;
			else
				x = -rand() % 3;
			this->particleObj->SetPosition(i, x, y, z);

			///初始化速度
			vx = 0.00001 * (rand() % 100);
			vy = 0.0000002 * (rand() % 28000);
			vz = 0;
			this->particleObj->SetVelocity(i, vx, vy, vz);

			///初始化加速度
			ax = 0;
			ay = 0.000005f;
			az = 0;
			this->particleObj->SetAcceleration(i, ax, ay, az);

			///初始化生命周期
			lifetime = 100;
			this->particleObj->SetLifeTime(i, lifetime);

			///消失速度
			dec = 0.005 * (rand() % 50);
			this->particleObj->SetDec(i, dec);

			///初始化大小
			this->particleObj->SetSize(i, 0.03f);
		}
		return true;
	}

	/** 更新粒子 */
	void UpdateSnow()
	{
		/** 更新位置 */
		x += (vx * 5);
		y -= vy;

		/** 更新速度 */
		vy += ay;

		/** 更新生存时间 */
		lifetime -= dec;

		if (x > 3)
			x = -2;

		/** 如果粒子消失或生命结束 */
		if (y <= -1 || lifetime <= 0)
		{
			/** 初始化位置 */
			x = 0.1f * (rand() % 50) - 2.5f;
			y = 2 + 0.1f * (rand() % 2);
			if ((int)x % 2 == 0)
				z = rand() % 6;
			else
				z = -rand() % 3;

			/** 初始化速度 */
			vx = (float)(0.00001 * (rand() % 100));
			vy = (float)(0.0000002 * (rand() % 28000));
			vz = 0;

			/** 初始化加速度 */
			ax = 0;
			ay = 0.000005f;
			az = 0;
			lifetime = 100;
			dec = 0.005 * (rand() % 50);
		}
	}

	void DrawParticle()
	{
		/** 绑定纹理 */
		glBindTexture(GL_TEXTURE_2D, ResM.getTexture("snow")->getTexture());

		for (int i = 0; i < particleObj->GetNumOfParticle(); ++i)
		{
			/** 获得粒子的所有属性 */
			particleObj->GetAll(i, r, g, b, x, y, z, vx, vy, vz, ax, ay, az, size, lifetime, dec);
			glLoadIdentity();
			glTranslatef(0.0f, 0.0f, -6.0f);
			glColor4ub(r, g, b, 255);
			glNormal3f(0.0f, 0.0f, 1.0f);   /**< 定义法线方向 */
			/** 画出粒子 */
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x - size, y - size, z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x - size, y + size, z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x + size, y + size, z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x + size, y - size, z);
			glEnd();

			/** 更新粒子属性 */
			UpdateSnow();
			particleObj->SetAll(i, r, g, b, x, y, z, vx, vy, vz, ax, ay, az, size, lifetime, dec);
		}
		glutPostRedisplay();//发送渲染请求
	}

	/** 初始化 */
	void init(void)
	{
		/** 用户自定义的初始化过程 */
		glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
		glClearDepth(1.0f);//指定深度缓冲区的清除值(即将深度缓冲区里的值设置为1)
		glDepthFunc(GL_LEQUAL);//指定用于深度缓冲比较值(即新进像素深度值与原来的1比较，<=则通过，否则丢弃)
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_SMOOTH);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glEnable(GL_TEXTURE_2D);             /**< 开启纹理映射 */
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);    /**< 设置混合因子获得半透明效果 */
		glEnable(GL_BLEND);                  /**< 启用混和 */

		/**  载入纹理 */
		if (!LoadTextures())
		{
			printf("bmp load failed");
			exit(-1);
		}

		/** 创建500个粒子 */
		particleObj->Create(500);

		/** 初始化粒子 */
		InitSnow();
	}

private:
	CParticle* particleObj;
	/** 用来设置粒子的属性值 */
	float x, y, z, vx, vy, vz, ax, ay, az, size, lifetime, dec;
	int r, g, b;
};

#endif
