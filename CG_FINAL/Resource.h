#ifndef GAME_RESOURCE_h
#define GAME_RESOURCE_h

// GLEW/GLAD
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
// OTHER
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>

#include "Shader.h"
#include "Texture.h"
#include "model.h"

class ResourceManager {
private:
	std::map<std::string, Shader*> shaderList;
	std::map<std::string, MY_Texture*> textureList;
	std::map<std::string, Model*> modelList;
public:
	void loadShader(std::string shaderName, const char* vertex, const char* fragment) {
		Shader* temp_shader = new Shader(vertex, fragment);
		this->shaderList.insert(std::pair<std::string, Shader*>(shaderName, temp_shader));
	}
	
	Shader* getShader(std::string shaderName) {
		return this->shaderList[shaderName];
	}
	
	void loadTexture(std::string textureName, char const * path) {
		MY_Texture* temp_texture = new MY_Texture(path);
		this->textureList.insert(std::pair<std::string, MY_Texture*>(textureName, temp_texture));
	}
	
	MY_Texture* getTexture(std::string textureName) {
		return this->textureList[textureName];
	}

	void loadModel(std::string modelName, const char* path) {
		Model* temp_model = new Model(path);
		this->modelList.insert(std::pair<std::string, Model*>(modelName, temp_model));
	}

	Model* getModel(std::string modelName) {
		return this->modelList[modelName];
	}
};
#endif // !HW7_h