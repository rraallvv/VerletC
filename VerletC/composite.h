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

// Composite -- a high level object used within the scene (ball, bridge, cloth, etc..)

#pragma once

#include "particle.h"
#include "constraint.h"

#include <vector>

using namespace std;

struct Composite {
	Particles particles;
	Constraints constraints;
	
	Composite(){}
	
	PinConstraint* pin(int index, Vec2 pos) {
		PinConstraint* pc = new PinConstraint(particles[index], pos);
		constraints.push_back(pc);
		return pc;
	}
	
	PinConstraint* pin(int index) {
		PinConstraint* pc = new PinConstraint(particles[index], particles[index]->pos);
		constraints.push_back(pc);
		return pc;
	}
	
	virtual void drawParticles() {
		int i;
		for (i=0; i<particles.size(); i++)
			particles[i]->draw();
	}
	
	virtual void drawConstraints() {
		int i;
		for (i=0; i<constraints.size(); i++)
			constraints[i]->draw();
	}
	
	virtual void update(float dt) {
	}
	
	~Composite() {
		int c, p;
		
		for (c = 0; c < constraints.size(); c++)
			delete constraints[c];
		
		constraints.clear();
		
		for (p = 0; p < particles.size(); p++)
			delete particles[p];
		
		particles.clear();
	}
};

typedef vector<Composite*> Composites;
