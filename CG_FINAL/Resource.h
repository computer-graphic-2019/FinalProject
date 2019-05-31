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


class ResourceManager {
private:
	std::map<std::string, Shader*> shaderList;
	std::map<std::string, Texture*> textureList;
public:
	void loadShader(std::string shaderName, const char* vertex, const char* fragment) {
		Shader* temp_shader = new Shader(vertex, fragment);
		this->shaderList.insert(std::pair<std::string, Shader*>(shaderName, temp_shader));
	}
	
	Shader* getShader(std::string shaderName) {
		return this->shaderList[shaderName];
	}
	
	void loadTexture(std::string textureName, char const * path) {
		Texture* temp_texture = new Texture(path);
		this->textureList.insert(std::pair<std::string, Texture*>(textureName, temp_texture));
	}
	
	Texture* getTexture(std::string textureName) {
		return this->textureList[textureName];
	}
};
#endif // !HW7_h