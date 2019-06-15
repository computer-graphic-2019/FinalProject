#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 aColor;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec3 ourColor;
void main()
{
   gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
   ourColor = aColor;
};