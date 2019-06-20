#ifndef GAME_TOOLS_H
#define GAME_TOOLS_H

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

#include "PhysicsEngine.h"

#include <vector>

extern unsigned int SCR_WIDTH, SCR_HEIGHT;
extern ResourceManager ResM;
extern GameMove moveController;
extern PhysicsEngine physicsEngine;

extern std::map<std::string, GameObject> targetList;
extern std::map<std::string, GameObject> movingTargetList;
extern std::map<std::string, GameObject> explodeTargeList;
extern std::map<std::string, bool> explodeTargeRec;
extern std::deque<std::string> recoverList;

int numOfTree = 1;
int numOfTree3 = 1;
int numOfGrass = 150;
int numOfStone = 1;

int coverWidth = 50;
int coveLength = 50;

class GameTools {
private:
	// shading
	GLuint depthMapFBO, depthMap;
	GLuint SHADOW_WIDTH, SHADOW_HEIGHT;
	glm::mat4 lightSpaceMatrix;
	glm::vec3 ambient_light, diffuse_light, specular_light;
	// random trees
	std::vector<float> treeScale, treeX, treeZ;
	std::vector<float> tree3Scale, tree3X, tree3Z;
	std::vector<float> grassX, grassZ;
	std::vector<float> stoneX, stoneZ;
public:
	GameTools(glm::vec3 light, float ambient, float diffuse, float specular) {
		// 初始化光照参数
		this->ambient_light = ambient * light;
		this->diffuse_light = diffuse * light;
		this->specular_light = specular * light;

		// 加载着色器
		ResM.loadShader("debug", "./ShaderCode/debug.vs", "./ShaderCode/debug.fs");
		ResM.loadShader("depthShader", "./ShaderCode/3.depth_mapping.vs", "./ShaderCode/3.depth_mapping.fs");
		ResM.loadShader("model", "./ShaderCode/3.phong_shading.vs", "./ShaderCode/3.phong_shading.fs", "./ShaderCode/4.explode_shading.gs");

		// 加载模型
		ResM.loadModel("place", "./models/place/scene2.obj");
		ResM.loadModel("target", "./models/target/target.obj");
		ResM.loadModel("explodeTarget", "./models/explodeTarget/explodeTarget.obj");
		ResM.loadModel("tree", "./models/scene/tree.obj");
		ResM.loadModel("tree3", "./models/scene/tree3.obj");
		ResM.loadModel("grass", "./models/scene/grass.obj");
		ResM.loadModel("stone", "./models/scene/stone.obj");

		// 初始化阴影贴图
		SHADOW_WIDTH = 4096;
		SHADOW_HEIGHT = 4096;
		glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);	

		// 随机位置参数
		for (int i = 0; i < numOfTree; i++) {
			int x = rand() % (2 * coveLength) - coveLength;
			int z = rand() % (2 * coverWidth) - coverWidth;
			float scale = rand() % 20 / (float)40 + 1.0;
			treeX.push_back(x);
			treeZ.push_back(z);
			treeScale.push_back(scale);
			//physicsEngine.setSceneInnerBoundary(glm::vec3(x - 2.0f, 2.0f, z - 2.0f), glm::vec3(x + 2.0f, 2.0f, z + 2.0f));
		}
		for (int i = 0; i < numOfTree3; i++) {
			int x = rand() % (2 * coveLength) - coveLength;
			int z = rand() % (2 * coverWidth) - coverWidth;
			float scale = rand() % 20 / (float)40 + 1.0;
			tree3X.push_back(x);
			tree3Z.push_back(z);
			tree3Scale.push_back(scale);
			//physicsEngine.setSceneInnerBoundary(glm::vec3(x - 2.0f, 5.0f, z - 2.0f), glm::vec3(x + 2.0f, 5.0f, z + 2.0f));
		}
		for (int i = 0; i < numOfGrass; i++) {
			grassX.push_back(rand() % (2 * coveLength) - coveLength);
			grassZ.push_back(rand() % (2 * coverWidth) - coverWidth);
		}
		
		for (int i = 0; i < numOfStone; i++) {
			int x = rand() % (2 * coveLength) - coveLength;
			int z = rand() % (2 * coverWidth) - coverWidth;
			float scale = rand() % 20 / (float)40 + 1.0;
			stoneX.push_back(x);
			stoneZ.push_back(z);
		}

		// set boundary
		// 设置地板碰撞盒
		physicsEngine.setSceneInnerBoundary(glm::vec3(-60.0f, 0.0f, -60.0f), glm::vec3(60.0f, 0.0f, 60.0f));
		physicsEngine.setSceneOuterBoundary(glm::vec2(-60.0f, -60.0f), glm::vec2(60.0f, 60.0f));

		// 阶梯
		int x = -34, z = -16, y = 1;
		for (int i = 0; i < 4; i++) {
			physicsEngine.setSceneInnerBoundary(glm::vec3(x - 6.0f, y - 5.0f, z - 2.0f), glm::vec3(x + 6.0f, y + 1.0f, z + 2.0f));
			z += 5;
			y += 6;
		}
		z = 16;
		y = 1;
		for (int i = 0; i < 4; i++) {
			physicsEngine.setSceneInnerBoundary(glm::vec3(x - 6.0f, y - 5.0f, z - 2.0f), glm::vec3(x + 6.0f, y + 1.0f, z + 2.0f));
			z -= 5;
			y += 6;
		}

		// 中部平台
		glm::vec3 platform(16.0f, 10.0f, 0.0f);
		physicsEngine.setSceneInnerBoundary(glm::vec3(platform.x - 44.0f, platform.y - 10.0f, platform.z - 18.0f), 
			glm::vec3(platform.x + 44.0f, platform.y + 10.0f, platform.z + 18.0f));
	}
	// 深度贴图
	void RenderDepthMap(glm::vec3 lightPos) {
		// bind data
		Shader* shader = ResM.getShader("depthShader");
		shader->use();
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 lightProjection = glm::ortho(-120.0f, 120.0f, -120.0f, 120.0f, 0.0f, 300.0f);
		lightSpaceMatrix = lightProjection * lightView;
		shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		// clear
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		// render
		RenderObject(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// 实际场景
	void RenderScene(glm::vec3 lightPos) {
		// bind data
		Shader* shader = ResM.getShader("model");
		shader->use();
		glm::vec3 viewPos = moveController.getHumanCamera()->getPosition();
		glm::mat4 view = moveController.getHumanCamera()->getView();
		glm::mat4 projection = glm::perspective(glm::radians(moveController.getHumanCamera()->getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		shader->setInt("shadowMap", 31);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);
		shader->setVec3("viewPos", viewPos);
		shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

		shader->setVec3("light.ambient", ambient_light);
		shader->setVec3("light.diffuse", diffuse_light);
		shader->setVec3("light.specular", specular_light);
		shader->setVec3("light.position", lightPos);

		shader->setBool("isExplode", false);

		// clear
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		// render
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		RenderObject(shader);
	}

	// 渲染所有物体
	void RenderObject(Shader* shader) {
		glm::mat4 model;

		// 地板
		model = glm::mat4(1.0f);
		shader->setMat4("model", model);
		ResM.getModel("place")->Draw(*shader);

		// 靶子
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, -10.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		shader->setMat4("model", model);
		ResM.getModel("target")->Draw(*shader);

		// 两种树
		for (int i = 0; i < numOfTree; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(treeX[i], 0.0f, treeZ[i]));
			model = glm::scale(model, glm::vec3(treeScale[i], treeScale[i], treeScale[i]));
			//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			shader->setMat4("model", model);
			ResM.getModel("tree")->Draw(*shader);
		}
		for (int i = 0; i < numOfTree3; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(tree3X[i], 0.0f, tree3Z[i]));
			model = glm::scale(model, glm::vec3(tree3Scale[i], tree3Scale[i], tree3Scale[i]));
			//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			shader->setMat4("model", model);
			ResM.getModel("tree3")->Draw(*shader);
		}

		// 草
		for (int i = 0; i < numOfGrass; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(grassX[i], 0.0f, grassZ[i]));
			model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
			//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			shader->setMat4("model", model);
			ResM.getModel("grass")->Draw(*shader);
		}

		// 石头
		for (int i = 0; i < numOfStone; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(stoneX[i], 0.0f, stoneZ[i]));
			model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
			//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			shader->setMat4("model", model);
			ResM.getModel("stone")->Draw(*shader);
		}

		// 爆炸靶子
		for (std::map<std::string, bool>::iterator ptr = explodeTargeRec.begin(); ptr != explodeTargeRec.end(); ptr++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 5.0f, -10.0f));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			shader->setMat4("model", model);
			if (ptr->second) {
				shader->setBool("isExplode", true);
				ResM.getModel("explodeTarget")->Draw((*shader));
			}
			else {
				ResM.getModel("explodeTarget")->Draw((*shader));
			}
		}
	}

	void testMap(GLFWwindow* window) {
		Shader* shader = ResM.getShader("debug");
		shader->use();
		//shader->setFloat("near_plane", near_plane);
		//shader->setFloat("far_plane", far_plane);
		shader->setInt("shadowMap", 0);
		// clear
		glfwSetWindowSize(window, SHADOW_WIDTH, SHADOW_HEIGHT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		GLuint quadVAO, quadVBO;

		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);
	}
	
};

#endif