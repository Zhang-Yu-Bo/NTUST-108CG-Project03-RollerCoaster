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
	GLfloat xpos;					//(xpos,ypos,zpos)為particle的position
	GLfloat ypos;
	GLfloat zpos;
	GLfloat xspeed;					//(xspeed,yspeed,zspeed)為particle的speed 
	GLfloat yspeed;
	GLfloat zspeed;
	GLfloat r;						//(r,g,b)為particle的color
	GLfloat g;
	GLfloat b;
	GLfloat life;					// particle的壽命 
	GLfloat fade;					// particle的衰減速度
	GLfloat size;					// particle的大小  
	GLbyte    bFire;
	GLbyte    nExpl;				//哪種particle效果  
	GLbyte    bAddParts;			// particle是否含有尾巴
	GLfloat   AddSpeed;				//尾巴粒子的加速度  
	GLfloat   AddCount;				//尾巴粒子的增加量  
	tag_PARTICLE* pNext;			//下一particle 
	tag_PARTICLE* pPrev;			//上一particle   
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

