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

struct Cloth : public Composite {
	float min;
	int segments;
	
	Cloth(VerletJS* sim, Vec2 origin, float width, float height, int segments, int pinMod, float stiffness):segments(segments) {
		float xStride = width/segments;
		float yStride = height/segments;
		
		min = fmin(width,height);
		
		int x,y;
		for (y=0;y<segments;++y) {
			for (x=0;x<segments;++x) {
				float px = origin.x + x*xStride - width/2 + xStride/2;
				float py = origin.y + y*yStride - height/2 + yStride/2;
				particles.push_back(new Particle(Vec2(px, py)));
				
				if (x > 0)
					constraints.push_back(new DistanceConstraint(particles[y*segments+x], particles[y*segments+x-1], stiffness));
				
				if (y > 0)
					constraints.push_back(new DistanceConstraint(particles[y*segments+x], particles[(y-1)*segments+x], stiffness));
			}
		}
		
		for (x=0;x<segments;++x) {
			if (x%pinMod == 0)
				pin(x);
		}
		
		sim->composites.push_back(this);
	}
	
	void drawParticles() {
		// do nothing for particles
	}
	
	void drawConstraints() {
		float stride = min/segments;
		int x,y;
		for (y=1;y<segments;++y) {
			for (x=1;x<segments;++x) {
				
				int i1 = (y-1)*segments+x-1;
				int i2 = (y)*segments+x;
				
				float off = particles[i2]->pos.x - particles[i1]->pos.x;
				off += particles[i2]->pos.y - particles[i1]->pos.y;
				off *= 0.25;
				
				float coef = fabsf(off)/stride;
				if (coef > 1.0)
					coef = 1.0;
				
				glColor4f(coef, 0, 1.0-coef, lerp(0.25,1.0,coef));
				
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				
				glBegin(GL_POLYGON);
				{
					glVertex2f(particles[i1]->pos.x, particles[i1]->pos.y);
					glVertex2f(particles[i1+1]->pos.x, particles[i1+1]->pos.y);
					glVertex2f(particles[i2]->pos.x, particles[i2]->pos.y);
					glVertex2f(particles[i2-1]->pos.x, particles[i2-1]->pos.y);
					
				}glEnd();
				
				glDisable(GL_BLEND);
			}
		}
		
		int c;
		for (c=0; c<constraints.size(); c++) {
			if (constraints[c]->type & Constraint::PIN) {
				PinConstraint* point = (PinConstraint*)constraints[c];
				glColor4ub(255, 255, 255, 255);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				drawCircle(point->pos, 1.2f);
			}
		}
	}
};
