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

extern unsigned int SCR_WIDTH, SCR_HEIGHT;
extern ResourceManager ResM;
extern GameMove moveController;

class GameTools {
private:
	GLuint depthMapFBO, depthMap;
	GLuint SHADOW_WIDTH, SHADOW_HEIGHT;
	glm::mat4 lightSpaceMatrix;
	glm::vec3 ambient_light, diffuse_light, specular_light;
public:
	GameTools(glm::vec3 light, float ambient, float diffuse, float specular) {
		// 初始化光照参数
		this->ambient_light = ambient * light;
		this->diffuse_light = diffuse * light;
		this->specular_light = specular * light;
		// 加载深度着色器
		ResM.loadShader("debug", "./ShaderCode/debug.vs", "./ShaderCode/debug.fs");
		ResM.loadShader("depthShader", "./ShaderCode/3.depth_mapping.vs", "./ShaderCode/3.depth_mapping.fs");
		// 初始化阴影贴图
		SHADOW_WIDTH = 1024;
		SHADOW_HEIGHT = 1024;
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
	}
	// 深度贴图
	void RenderDepthMap(glm::vec3 lightPos) {
		// bind data
		Shader* shader = ResM.getShader("depthShader");
		shader->use();
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 50.0f, 150.0f);
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

		// clear
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		RenderObject(shader);
	}

	// 场景渲染
	void RenderObject(Shader* shader) {
		// floor
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
		shader->setMat4("model", model);
		ResM.getModel("place")->Draw(*shader);
		// target
		model = glm::mat4(1.0f);
		//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		shader->setMat4("model", model);
		ResM.getModel("target")->Draw(*shader);
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
	/*
	void RenderScene1(Shader* shader) {
		ResM.getShader("model")->use();

		glm::mat4 projection = glm::perspective(glm::radians(moveController.getHumanCamera()->getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = moveController.getHumanCamera()->getView();
		glm::vec3 viewPos = moveController.getHumanCamera()->getPosition();

		ResM.getShader("model")->setMat4("view", view);
		ResM.getShader("model")->setMat4("projection", projection);
		ResM.getShader("model")->setVec3("viewPos", viewPos);
		ResM.getShader("model")->setVec3("lightDirection", cos(glfwGetTime()), -0.5f, sin(glfwGetTime()));

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f)); // translate it down so it's at the center of the scene
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ResM.getShader("model")->setMat4("model", model);
		ResM.getModel("place")->Draw((*ResM.getShader("model")));

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, -100.0f));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ResM.getShader("model")->setMat4("model", model);
		ResM.getModel("target")->Draw((*ResM.getShader("model")));

		// explode target
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(7.0f, 3.0f, -100.0f));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		for (std::map<std::string, bool>::iterator ptr = explodeTargeRec.begin(); ptr != explodeTargeRec.end(); ptr++) {
			if (ptr->second) {
				ResM.getShader("explodeModel")->use();
				ResM.getShader("explodeModel")->setMat4("gunRotate", glm::mat4(1.0f));
				ResM.getShader("explodeModel")->setMat4("projection", projection);
				ResM.getShader("explodeModel")->setMat4("view", view);
				ResM.getShader("explodeModel")->setVec3("viewPos", viewPos);
				ResM.getShader("explodeModel")->setVec3("lightDirection", cos(glfwGetTime()), -0.5f, sin(glfwGetTime()));
				// add time component to geometry shader in the form of a uniform
				ResM.getShader("explodeModel")->setFloat("time", 1.0f); //爆炸效果改这里
				ResM.getShader("explodeModel")->setMat4("model", model);
				ResM.getModel("explodeTarget")->Draw((*ResM.getShader("explodeModel")));
			}
			else {
				ResM.getShader("model")->setMat4("model", model);
				ResM.getModel("explodeTarget")->Draw((*ResM.getShader("model")));
			}
		}
	}
	*/
};

#endif