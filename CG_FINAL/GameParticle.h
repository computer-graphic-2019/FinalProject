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
			///��ʼ����ɫ����ɫ��
			r = 255;
			g = 255;
			b = 255;
			this->particleObj->SetColor(i, r, g, b);

			///��ʼ������
			x = 0.1f * (rand() % 50) - 2.5f;
			y = 2 + 0.1f * (rand() % 2);
			if ((int)x % 2 == 0)
				z = rand() % 6;
			else
				x = -rand() % 3;
			this->particleObj->SetPosition(i, x, y, z);

			///��ʼ���ٶ�
			vx = 0.00001 * (rand() % 100);
			vy = 0.0000002 * (rand() % 28000);
			vz = 0;
			this->particleObj->SetVelocity(i, vx, vy, vz);

			///��ʼ�����ٶ�
			ax = 0;
			ay = 0.000005f;
			az = 0;
			this->particleObj->SetAcceleration(i, ax, ay, az);

			///��ʼ����������
			lifetime = 100;
			this->particleObj->SetLifeTime(i, lifetime);

			///��ʧ�ٶ�
			dec = 0.005 * (rand() % 50);
			this->particleObj->SetDec(i, dec);

			///��ʼ����С
			this->particleObj->SetSize(i, 0.03f);
		}
		return true;
	}

	/** �������� */
	void UpdateSnow()
	{
		/** ����λ�� */
		x += (vx * 5);
		y -= vy;

		/** �����ٶ� */
		vy += ay;

		/** ��������ʱ�� */
		lifetime -= dec;

		if (x > 3)
			x = -2;

		/** ���������ʧ���������� */
		if (y <= -1 || lifetime <= 0)
		{
			/** ��ʼ��λ�� */
			x = 0.1f * (rand() % 50) - 2.5f;
			y = 2 + 0.1f * (rand() % 2);
			if ((int)x % 2 == 0)
				z = rand() % 6;
			else
				z = -rand() % 3;

			/** ��ʼ���ٶ� */
			vx = (float)(0.00001 * (rand() % 100));
			vy = (float)(0.0000002 * (rand() % 28000));
			vz = 0;

			/** ��ʼ�����ٶ� */
			ax = 0;
			ay = 0.000005f;
			az = 0;
			lifetime = 100;
			dec = 0.005 * (rand() % 50);
		}
	}

	void DrawParticle()
	{
		/** ������ */
		glBindTexture(GL_TEXTURE_2D, ResM.getTexture("snow")->getTexture());

		for (int i = 0; i < particleObj->GetNumOfParticle(); ++i)
		{
			/** ������ӵ��������� */
			particleObj->GetAll(i, r, g, b, x, y, z, vx, vy, vz, ax, ay, az, size, lifetime, dec);
			glLoadIdentity();
			glTranslatef(0.0f, 0.0f, -6.0f);
			glColor4ub(r, g, b, 255);
			glNormal3f(0.0f, 0.0f, 1.0f);   /**< ���巨�߷��� */
			/** �������� */
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x - size, y - size, z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x - size, y + size, z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x + size, y + size, z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x + size, y - size, z);
			glEnd();

			/** ������������ */
			UpdateSnow();
			particleObj->SetAll(i, r, g, b, x, y, z, vx, vy, vz, ax, ay, az, size, lifetime, dec);
		}
		glutPostRedisplay();//������Ⱦ����
	}

	/** ��ʼ�� */
	void init(void)
	{
		/** �û��Զ���ĳ�ʼ������ */
		glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
		glClearDepth(1.0f);//ָ����Ȼ����������ֵ(������Ȼ��������ֵ����Ϊ1)
		glDepthFunc(GL_LEQUAL);//ָ��������Ȼ���Ƚ�ֵ(���½��������ֵ��ԭ����1�Ƚϣ�<=��ͨ����������)
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_SMOOTH);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glEnable(GL_TEXTURE_2D);             /**< ��������ӳ�� */
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);    /**< ���û�����ӻ�ð�͸��Ч�� */
		glEnable(GL_BLEND);                  /**< ���û�� */

		/**  �������� */
		if (!LoadTextures())
		{
			printf("bmp load failed");
			exit(-1);
		}

		/** ����500������ */
		particleObj->Create(500);

		/** ��ʼ������ */
		InitSnow();
	}

private:
	CParticle* particleObj;
	/** �����������ӵ�����ֵ */
	float x, y, z, vx, vy, vz, ax, ay, az, size, lifetime, dec;
	int r, g, b;
};

#endif