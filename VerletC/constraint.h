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

// DistanceConstraint -- constrains to initial distance
// PinConstraint -- constrains to static/fixed point
// AngleConstraint -- constrains 3 particles to an angle

#pragma once

#include "particle.h"
#include "util.h"

#include <math.h>

struct Constraint {
	enum Type{
		UNKNOWN  = 0,
		PIN		 = 1<<0,
		DISTANCE = 1<<1,
		ANGLE	 = 1<<2
	};
	
	Type type;
	
	virtual void relax(float stepCoef) = 0;
	virtual void draw() = 0;
	
	Constraint(Type type): type(type) {}
	
	virtual ~Constraint() {}
};

typedef vector<Constraint*> Constraints;

struct DistanceConstraint : public Constraint {
	Particle* a;
	Particle* b;
	float distance;
	float stiffness;
	
	DistanceConstraint(Particle* a, Particle* b, float stiffness)
	: Constraint(DISTANCE), a(a), b(b), stiffness(stiffness) {
		distance = (a->pos-b->pos).length();
	}
	
	DistanceConstraint(Particle* a, Particle* b, float stiffness, float distance)
	: Constraint(DISTANCE), a(a), b(b), stiffness(stiffness), distance(distance) {
	}
	
	void relax(float stepCoef) {
		Vec2 normal = a->pos-b->pos;
		float m = normal.length2();
		normal *= ((distance*distance - m)/m)*stiffness*stepCoef;
		a->pos += normal;
		b->pos -= normal;
	}
	
	void draw() {
		glLineWidth(1.5);
		glColor3ub(216, 221, 226);
		drawLine(a->pos, b->pos);
	}
	
	~DistanceConstraint() {}
};

struct PinConstraint : public Constraint, public Draggable {
	Vec2 pos;
	Particle* a;
	
	PinConstraint(Particle* a, Vec2 pos): Constraint(PIN), pos(pos), a(a) {}
	
	void relax(float stepCoef) {
		a->pos = pos;
	}
	
	Vec2 getPos() {
		return pos;
	}
	
	void setPos(Vec2 p) {
		pos = p;
	}
	
	void draw() {
		glLineWidth(1.5);
		glColor4ub(0,153,255,26);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawCircle(pos, 6.0f);
		
		glDisable(GL_BLEND);
	}
	
	~PinConstraint() {}
};

struct AngleConstraint : public Constraint {
	Particle* a;
	Particle* b;
	Particle* c;
	float angle;
	float stiffness;
	
	AngleConstraint(Particle* a, Particle* b, Particle* c, float stiffness): Constraint(ANGLE), a(a), b(b), c(c), stiffness(stiffness) {
		angle = b->pos.angle2(a->pos, c->pos);
	}
	
	void relax(float stepCoef) {
		float newAngle = b->pos.angle2(a->pos, c->pos);
		float diff = newAngle - angle;
		
		if (diff <= -M_PI)
			diff += 2.0f*M_PI;
		else if (diff >= M_PI)
			diff -= 2.0f*M_PI;
		
		diff *= stepCoef*stiffness;
		
		a->pos = a->pos.rotate(b->pos, diff);
		c->pos = c->pos.rotate(b->pos, -diff);
		b->pos = b->pos.rotate(a->pos, diff);
		b->pos = b->pos.rotate(c->pos, -diff);
	}
	
	void draw() {
		glColor4ub(255,255,0,51);
		glLineWidth(5);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glBegin(GL_LINES);
		{
			glVertex2f(b->pos.x, b->pos.y);
			glVertex2f(a->pos.x, a->pos.y);
			glVertex2f(b->pos.x, b->pos.y);
			glVertex2f(c->pos.x, c->pos.y);
			
		}glEnd();
		
		glLineWidth(1);
		
		glDisable(GL_BLEND);
	}
	
	~AngleConstraint() {}
};
