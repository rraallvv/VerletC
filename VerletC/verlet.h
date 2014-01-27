/*
 Copyright 2013 Sub Protocol and other contributors
 http://subprotocol.com/
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "composite.h"

using namespace std;

struct VerletJS {
	int width;
	int height;
	
	Vec2 mousePos = Vec2(0,0);
	bool mouseDown = false;
	Draggable* draggedEntity = NULL;
	float selectionRadius = 20.0f;
	GLubyte highlightColor[3] = { 0x4F, 0x54, 0x5C };
	
	// simulation params
	Vec2 gravity = Vec2(0,0.2);
	float friction = 0.99;
	float groundFriction = 0.8;
	
	// holds composite entities
	Composites composites;
	
	VerletJS(int width, int height): width(width), height(height) {}
	
	void bounds(Particle *particle) {
		if (particle->pos.y > height-1)
			particle->pos.y = height-1;
		
		if (particle->pos.x < 0)
			particle->pos.x = 0;
		
		if (particle->pos.x > width-1)
			particle->pos.x = width-1;
	}
	
	Draggable* nearestEntity() {
		int c, i;
		float d2Nearest = 0.0f;
		Draggable* entity = NULL;
		Constraints* constraintsNearest = NULL;
		
		// find nearest point
		for (c = 0; c < composites.size(); c++) {
			Particles& particles = composites[c]->particles;
			for (i = 0; i < particles.size(); i++) {
				float d2 = particles[i]->pos.dist2(mousePos);
				if (d2 <= selectionRadius*selectionRadius && (entity == NULL || d2 < d2Nearest)) {
					entity = particles[i];
					constraintsNearest = &(composites[c]->constraints);
					d2Nearest = d2;
				}
			}
		}
		
		// search for pinned constraints for this entity
		if (constraintsNearest) {
			Constraints& constraints = *constraintsNearest;
			for (i = 0; i < constraints.size(); i++) {
				if (constraints[i]->type & Constraint::PIN && static_cast<PinConstraint*>(constraints[i])->a == entity)
					entity = static_cast<Draggable*>(static_cast<PinConstraint*>(constraints[i]));
			}
		}
		return entity;
	}
	
	void onMouseClick( int button, int state, int x, int y ) {
		mousePos.x = x;
		mousePos.y = y;
		if (state == GLUT_DOWN) {
			mouseDown = true;
			Draggable* nearest = nearestEntity();
			if (nearest)
				draggedEntity = nearest;
		} else {
			mouseDown = false;
			draggedEntity = NULL;
		}
	};
	
	void onMouseMove( int x, int y ) {
		mousePos.x = x;
		mousePos.y = y;
	};
	
	void update(float dt, int step = 16) {
		int i, j, c;
		
		for (c = 0; c < composites.size(); c++) {
			composites[c]->update(dt);
			
			for (i = 0; i < composites[c]->particles.size(); i++) {
				Particles& particles = composites[c]->particles;
				
				// calculate velocity
				Vec2 velocity = (particles[i]->pos-particles[i]->lastPos)*friction;
				
				// ground friction
				if (particles[i]->pos.y >= height-1 && velocity.length2() > 0.000001) {
					float m = velocity.length();
					velocity.x /= m;
					velocity.y /= m;
					velocity *= m*groundFriction;
				}
				
				// save last good state
				particles[i]->lastPos = particles[i]->pos;
				
				// gravity
				particles[i]->pos += gravity*60.0f*dt;
				
				// inertia
				particles[i]->pos += velocity;
			}
		}
		
		// handle dragging of entities
		if (draggedEntity)
			draggedEntity->setPos(mousePos);
		
		// relax
		float stepCoef = 1.0f/step;
		for (c = 0; c < composites.size(); c++) {
			Constraints& constraints = composites[c]->constraints;
			for (i=0;i<step;++i)
				for (j=0; j<constraints.size(); j++)
					constraints[j]->relax(stepCoef);
		}
		
		// bounds checking
		for (c=0; c<composites.size(); c++) {
			Particles& particles = composites[c]->particles;
			for (i=0; i<particles.size(); i++)
				bounds(particles[i]);
		}
	}
	
	void draw() {
		int i;
		glEnable( GL_POINT_SMOOTH );
		
		for (i=0; i<composites.size(); i++) {
			composites[i]->drawConstraints();
			composites[i]->drawParticles();
		}
		
		// highlight nearest / dragged entity
		Draggable* nearest = draggedEntity ? draggedEntity : nearestEntity();
		if (nearest) {
			glColor3ubv(highlightColor);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			drawCircle(nearest->getPos(), 8.0f);
		}
	}
	
	~VerletJS() {
		for (int c = 0; c < composites.size(); c++)
			delete composites[c];
		composites.clear();
	}
};
