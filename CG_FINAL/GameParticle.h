#ifndef GAMEPARTICLE_H
#define GAMEPARTICLE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Particle.h"
#include "GameResource.h"

extern ResourceManager ResM;

struct Particle
{
	float Type;
	glm::vec3 Pos;
	glm::vec3 Vel;
	float LifetimeMillis;
};

class ParticleSystem
{
public:
	ParticleSystem() {

	}

	~ParticleSystem() {
	}

	bool InitParticleSystem(const glm::vec3& Pos) {
		Particle Particles[MAX_PARTICLES];
		ZERO_MEM(Particles);
		Particles[0].Type = PARTICLE_TYPE_LAUNCHER;
		Particles[0].Pos = Pos;
		Particles[0].Vel = Vector3f(0.0f, 0.0001f, 0.0f);
		Particles[0].LifetimeMillis = 0.0f;
		glGenTransformFeedbacks(2, m_transformFeedback);
		glGenBuffers(2, m_particleBuffer);
		for (unsigned int i = 0; i < 2; i++) {
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
			glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
	}

	void Render(int DeltaTimeMillis, const Matrix4f& VP, const Vector3f& CameraPos) {
		m_time += DeltaTimeMillis;
		UpdateParticles(DeltaTimeMillis);
		RenderParticles(VP, CameraPos);
		m_currVB = m_currTFB;
		m_currTFB = (m_currTFB + 1) & 0x1;
	}
private:
	bool m_isFirst;
	unsigned int m_currVB;
	unsigned int m_currTFB;
	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];
	PSUpdateTechnique m_updateTechnique;
	BillboardTechnique m_billboardTechnique;
	RandomTexture m_randomTexture;
	Texture* m_pTexture;
	int m_time;
};

#endif
