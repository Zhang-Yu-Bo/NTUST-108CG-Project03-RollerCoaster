#pragma once
#include <QtOpenGL>  
#include <GL/GLU.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define MAX_PARTICLES 1000  
#define MAX_FIRES 5  

#ifndef _PARTICLE_API_H_
#define _PARTICLE_API_H_

typedef struct tag_PARTICLE {
	GLfloat xpos;					//(xpos,ypos,zpos)��particle��position
	GLfloat ypos;
	GLfloat zpos;
	GLfloat xspeed;					//(xspeed,yspeed,zspeed)��particle��speed 
	GLfloat yspeed;
	GLfloat zspeed;
	GLfloat r;						//(r,g,b)��particle��color
	GLfloat g;
	GLfloat b;
	GLfloat life;					// particle���ةR 
	GLfloat fade;					// particle���I��t��
	GLfloat size;					// particle���j�p  
	GLbyte    bFire;
	GLbyte    nExpl;				//����particle�ĪG  
	GLbyte    bAddParts;			// particle�O�_�t������
	GLfloat   AddSpeed;				//���ڲɤl���[�t��  
	GLfloat   AddCount;				//���ڲɤl���W�[�q  
	tag_PARTICLE* pNext;			//�U�@particle 
	tag_PARTICLE* pPrev;			//�W�@particle   
} Particle, *pParticle;


class ParticleAPI
{
public:
	ParticleAPI();
	~ParticleAPI();

	static pParticle Particles;
	static UINT Tick1, Tick2;
	static float DTick;
	static GLfloat grav;
	static UINT nOfFires;
	static GLuint textureID;

	static void ProcessParticles();
	static void DrawParticles();
private:
	static void AddParticle(Particle);
	static void DeleteParticle(pParticle*);
	static void DeleteAll(pParticle*);
	static void InitParticle(Particle&);
	static void Explosion1(Particle*);
	static void Explosion2(Particle*);
	static void Explosion3(Particle*);
	static void Explosion4(Particle*);
	static void Explosion5(Particle*);
	static void Explosion6(Particle*);
	static void Explosion7(Particle*);
};

#endif // !_PARTICLE_API_H_

