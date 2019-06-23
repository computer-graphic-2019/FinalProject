#ifndef PARTICLE_H
#define PARTICLE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"

float particleLife = 1.0;
float radius = 0.02;

struct Particle {
	glm::vec3 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleSystem
{
public:
	ParticleSystem() {}
	void init(Shader *shader, GLuint texture, GLuint amount)
	{
		this->shader = shader;
		this->texture = texture;
		this->amount = amount;
		this->lastUsedParticle = 0;
		// Set up mesh and attribute properties
		GLuint VBO;
		GLfloat particle_quad[] = {
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 0.0f, 1.0f, 0.0f
		};
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
		// Set mesh attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)3);
		glBindVertexArray(0);

		// Create this->amount default particle instances
		for (GLuint i = 0; i < this->amount; ++i) {
			this->particles.push_back(Particle());
		}	
	}
	// Update all particles
	void Update(GLfloat dt, glm::vec3 objectPos, glm::vec3 objectVel, GLuint newParticles)
	{
		// Add new particles 
		for (GLuint i = 0; i < newParticles; ++i)
		{
			int unusedParticle = this->firstUnusedParticle();
			this->respawnParticle(this->particles[unusedParticle], objectPos, objectVel);
		}
		// Update all particles
		for (GLuint i = 0; i < this->amount; ++i)
		{
			Particle &p = this->particles[i];
			p.Life -= dt; // reduce life
			if (p.Life > 0.0f)
			{
				p.Position += p.Velocity * dt;
				if (p.Color.b > 0.0f) {
					p.Color.b -= 2 * dt;
				}
				else {
					p.Color.g -= 2 * dt;
				}
				p.Color.a -= 0.5f * dt;
			}
		}
	}
	// Render all particles
	void Draw(glm::mat4 view, glm::mat4 projection)
	{
		// Use additive blending to give it a 'glow' effect
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		this->shader->use();
		glDisable(GL_CULL_FACE);
		for (Particle particle : this->particles)
		{
			if (particle.Life > 0.0f)
			{
				this->shader->setInt("sprite", 30);
				this->shader->setMat4("view", view);
				this->shader->setMat4("projection", projection);
				this->shader->setVec3("offset", particle.Position);
				this->shader->setVec4("color", particle.Color);
				glActiveTexture(GL_TEXTURE30);
				glBindTexture(GL_TEXTURE_2D, this->texture);
				glBindVertexArray(this->VAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);
			}
		}
		glEnable(GL_CULL_FACE);
		// Don't forget to reset to default blending mode
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
private:
	// State
	std::vector<Particle> particles;
	GLuint amount;
	// Render state
	Shader *shader;
	GLuint texture;
	GLuint VAO;
	GLuint lastUsedParticle;
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	GLuint firstUnusedParticle()
	{
		// First search from last used particle, this will usually return almost instantly
		for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
			if (this->particles[i].Life <= 0.0f) {
				lastUsedParticle = i;
				return i;
			}
		}
		// Otherwise, do a linear search
		for (GLuint i = 0; i < lastUsedParticle; ++i) {
			if (this->particles[i].Life <= 0.0f) {
				lastUsedParticle = i;
				return i;
			}
		}
		// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
		lastUsedParticle = 0;
		return 0;
	}
	// Respawns particle
	void respawnParticle(Particle &particle, glm::vec3 objectPos, glm::vec3 objectVel)
	{
		GLfloat randomX = ((rand() % 200) - 100) / 100.0f * radius;
		GLfloat randomY = ((rand() % 200) - 100) / 100.0f * radius;
		GLfloat randomZ = ((rand() % 200) - 100) / 100.0f * radius;
		//GLfloat init_color = 0.8 + ((rand() % 40) / 200.0f);
		particle.Position = objectPos + glm::vec3(randomX, randomY, randomZ);
		particle.Color = glm::vec4(1.0f);
		particle.Life = particleLife;
		particle.Velocity = 0.1f * (1.5f * objectVel + glm::normalize(glm::vec3(randomX, randomY, randomZ)));
	}
};

#endif
