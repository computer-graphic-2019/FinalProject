#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 texCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gunRotate;

void main()
{
    vs_out.FragPos = vec3(gunRotate * model * vec4(aPos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(gunRotate * model))) * aNormal;
    vs_out.texCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}