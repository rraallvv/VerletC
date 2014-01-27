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

// generic verlet entities

#pragma once

#include "composite.h"

struct Point : public Composite {
	Point(VerletJS* sim, Vec2 pos) {
		particles.push_back(new Particle(pos));
		sim->composites.push_back(this);
	}
};

struct LineSegments : public Composite {
	template<int N>
	LineSegments(VerletJS* sim, Vec2 (&vertices)[N], float stiffness) {
		int i;
		int count = N;
		
		for (i=0; i<count; i++) {
			particles.push_back(new Particle(vertices[i]));
			if (i > 0)
				constraints.push_back(new DistanceConstraint(particles[i], particles[i-1], stiffness));
		}
		
		sim->composites.push_back(this);
	}
};

struct Tire : public Composite {
	Tire(VerletJS* sim, Vec2 origin, float radius, int segments, float spokeStiffness, float treadStiffness) {
		float stride = (2*M_PI)/segments;
		int i;
		
		// particles
		for (i=0;i<segments;++i) {
			float theta = i*stride;
			particles.push_back(new Particle(Vec2(origin.x + cosf(theta)*radius, origin.y + sinf(theta)*radius)));
		}
		
		Particle* center = new Particle(origin);
		particles.push_back(center);
		
		// constraints
		for (i=0;i<segments;++i) {
			constraints.push_back(new DistanceConstraint(particles[i], particles[(i+1)%segments], treadStiffness));
			constraints.push_back(new DistanceConstraint(particles[i], center, spokeStiffness));
			constraints.push_back(new DistanceConstraint(particles[i], particles[(i+5)%segments], treadStiffness));
		}
		
		sim->composites.push_back(this);
	}
};
