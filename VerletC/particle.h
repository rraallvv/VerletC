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

// Particle -- just a point in space that responds to gravity.

#pragma once

#include "vec2.h"

#include <vector>

using namespace std;

struct Draggable {
	virtual Vec2 getPos() = 0;
	virtual void setPos(Vec2 p) = 0;
};

struct Particle : public Draggable {
	Vec2 pos;
	Vec2 lastPos;
	
	Particle(Vec2 pos): pos(pos), lastPos(pos) {}
	
	void draw() {
		glColor3ub(45, 173, 143);
		glPointSize(4);
		glBegin(GL_POINTS);{
			glVertex2f(pos.x, pos.y);
		}glEnd();
	}
	
	Vec2 getPos() {
		return pos;
	}
	
	void setPos(Vec2 p) {
		pos = p;
	}
};

typedef vector <Particle*> Particles;