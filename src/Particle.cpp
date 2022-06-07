//
// sueda - geometry edits Z. Wood
// 3/16
//

#include <iostream>
#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"


float randFloat(float l, float h)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * l + r * h;
}

Particle::Particle(vec3 start, vec3 partColor) :
	charge(1.0f),
	m(1.0f),
	d(0.0f),
	x(start),
	v(0.0f, 0.0f, 0.0f),
	lifespan(0.1f),
	tEnd(0.0f),
	scale(1.0f),
	color(partColor.x, partColor.y, partColor.z, 1.0f)
{
}

Particle::~Particle()
{
}

void Particle::load(vec3 start)
{
	// Random initialization
	rebirth(0.0f, start);
}

/* all particles born at the origin */
void Particle::rebirth(float t, vec3 start)
{
	charge = randFloat(0.0f, 1.0f) < 0.5 ? -1.0f : 1.0f;	
	m = 1.0f;
  	d = randFloat(0.0f, 0.02f);
	// x = start + vec3(randFloat(-60.5f, -40.5f),randFloat(135.5f, 150.5f),randFloat(-210.5f, -200.5f));
	x = start;
	v.x = randFloat(-0.27f, 0.3f);
	v.y = randFloat(-0.1f, 0.9f);
	v.z = randFloat(-0.3f, 0.27f);
	lifespan = randFloat(0.0f, 0.7f); 
	tEnd = t + lifespan;
	// scale = randFloat(1000.0f, 2000.0f);
	color.a = 0.8f;
}

void Particle::update(float t, float h, const vec3 &g, const vec3 start)
{
	if(t > tEnd) {
		rebirth(t, start);
	}

	//very simple updatema
	x += h*((g*(7 - (tEnd - t)))+v);
	
	//To do - how do you want to update the forces?
	color.a = (tEnd-t)/lifespan - 0.8;
	//std::cout << "FFF: " << color.a << "\n";
}
