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

struct TreeBranch : public DistanceConstraint {
	float p;
	
	TreeBranch(Particle* a, Particle* b, float stiffness)
	: DistanceConstraint(a, b, stiffness) {
	}
};

struct TreeLeaf : public Particle {
	bool leaf = false;
	TreeLeaf(Vec2 pos): Particle(pos) {}
};

struct Tree : public Composite {
	
	float lineCoef = 0.7;
	float branchLength;
	
	float debugDraw = false;
	
	float theta;
	
	Tree(VerletJS* sim, Vec2 origin, int depth, float branchLength, float segmentCoef, float theta): branchLength(branchLength), theta(theta) {
		Particle* base = new Particle(origin);
		Particle* root = new Particle(origin+Vec2(0,10));
		
		particles.push_back(base);
		particles.push_back(root);
		pin(0);
		pin(1);
		
		Particle* firstBranch = branch(base, 0, depth, segmentCoef, Vec2(0,-1));
		
		constraints.push_back(new AngleConstraint(root, base, firstBranch, 1));
		
		// animates the tree at the beginning
		float noise = 10;
		int i;
		for (i=0;i<particles.size();++i)
			particles[i]->pos += Vec2(floor(rand()/(float)RAND_MAX*noise), floor(rand()/(float)RAND_MAX*noise));
		
		sim->composites.push_back(this);
	}
	
	Particle* branch(Particle* parent, int i, int nMax, float coef, Vec2 normal) {
		TreeLeaf* particle = new TreeLeaf(parent->pos+(normal*branchLength*coef));
		particles.push_back(particle);
		
		TreeBranch* dc = new TreeBranch(parent, particle, lineCoef);
		dc->p = i/(float)nMax; // a hint for drawing
		constraints.push_back(dc);
		
		particle->leaf = !(i < nMax);
		
		if (i < nMax)
		{
			Particle* a = branch(particle, i+1, nMax, coef*coef, normal.rotate(Vec2(0,0), -theta));
			Particle* b = branch(particle, i+1, nMax, coef*coef, normal.rotate(Vec2(0,0), theta));
			
			float jointStrength = lerp(0.7, 0, i/nMax);
			constraints.push_back(new AngleConstraint(parent, particle, a, jointStrength));
			constraints.push_back(new AngleConstraint(parent, particle, b, jointStrength));
		}
		
		return particle;
	}
	
	void drawParticles() {
		if (debugDraw) {
			Composite::drawParticles();
			return;
		}
		
		int i;
		for (i=0;i<particles.size();++i) {
			TreeLeaf* particle = (TreeLeaf*)particles[i];
			if (particle->leaf) {
				glColor3ub(103, 157, 124);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				drawCircle(particle->pos, 25.0f);
			}
		}
	}
	
	void drawConstraints() {
		if (debugDraw) {
			Composite::drawConstraints();
			return;
		}
		
		int i;
		
		glColor3ub(84, 51, 36);
		
		for (i=0;i<constraints.size();++i) {
			TreeBranch* constraint = (TreeBranch*)constraints[i];
			if (constraint->type & Constraint::DISTANCE) {
				glLineWidth(lerp(10.0f,2.0f,constraint->p));
				drawLine(constraint->a->pos, constraint->b->pos);
			}
		}
	}
};
