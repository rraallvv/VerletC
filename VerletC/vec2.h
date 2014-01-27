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

// A simple 2-dimensional vector implementation

#pragma once

#include <iostream>
#include <string>
#include <math.h>

using namespace std;

struct Vec2 {
	float x = 0;
	float y = 0;
	
	Vec2() {}
	
	Vec2(float x, float y) :
	x(x), y(y)
	{}
	
	Vec2& operator=(const Vec2& v) {
		x = v.x;
		y = v.y;
		return *this;
	}
	
	Vec2& operator+=(const Vec2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	
	Vec2& operator-=(const Vec2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	
	Vec2& operator*=(const Vec2& v) {
		x *= v.x;
		y *= v.y;
		return *this;
	}
	
	Vec2& operator/=(const Vec2& v) {
		x /= v.x;
		y /= v.y;
		return *this;
	}
	
	Vec2& operator*=(const float& coef) {
		x *= coef;
		y *= coef;
		return *this;
	}
	
	Vec2 operator+(Vec2 v) {
		return Vec2(x + v.x, y + v.y);
	}
	
	Vec2 operator-(Vec2 v) {
		return Vec2(x - v.x, y - v.y);
	}
	
	Vec2 operator*(Vec2 v) {
		return Vec2(x * v.x, y * v.y);
	}
	
	Vec2 operator/(Vec2 v) {
		return Vec2(x / v.x, y / v.y);
	}
	
	Vec2 operator*(float coef) {
		return Vec2(x*coef, y*coef);
	}
	
	bool operator==(Vec2 v) {
		return x == v.x && y == v.y;
	}
	
	bool epsilonEquals(Vec2 v, float epsilon) {
		return fabsf(x - v.x) <= epsilon && fabsf(y - v.y) <= epsilon;
	}
	
	float length() {
		return sqrtf(x*x + y*y);
	}
	
	float length2() {
		return x*x + y*y;
	}
	
	float dist(Vec2 v) {
		return sqrtf(dist2(v));
	}
	
	float dist2(Vec2 v) {
		float dx = v.x - x;
		float dy = v.y - y;
		return dx*dx + dy*dy;
	}
	
	Vec2 normal() {
		float m = sqrtf(x*x + y*y);
		return Vec2(x/m, y/m);
	}
	
	float dot(Vec2 v) {
		return x*v.x + y*v.y;
	}
	
	float angle(Vec2 v) {
		return atan2f(x*v.y-y*v.x,x*v.x+y*v.y);
	}
	
	float angle2(Vec2 vLeft, Vec2 vRight) {
		return (vLeft-*this).angle(vRight-*this);
	}
	
	Vec2 rotate(Vec2 origin, float theta) {
		float dx = x - origin.x;
		float dy = y - origin.y;
		return Vec2(dx*cosf(theta) - dy*sinf(theta) + origin.x, dx*sinf(theta) + dy*cosf(theta) + origin.y);
	}
	
	operator string() const {
		return "(" + to_string(x) + ", " + to_string(y) + ")";
	}
};

void assert(const char* label, bool expression) {
	cout << "Vec2(" << label << "): " << (expression == true ? "PASS" : "FAIL") << "\n";
	if (expression != true)
		throw "assertion failed";
};

void  test_Vec2() {
	
	assert("equality", Vec2(5,3)==Vec2(5,3));
	assert("epsilon equality", (Vec2(1,2).epsilonEquals(Vec2(1.01,2.02), 0.03)));
	assert("epsilon non-equality", !(Vec2(1,2).epsilonEquals(Vec2(1.01,2.02), 0.01)));
	assert("addition", Vec2(1,1)+Vec2(2, 3) == Vec2(3, 4));
	assert("subtraction", Vec2(4,3)-Vec2(2, 1) == Vec2(2, 2));
	assert("multiply", Vec2(2,4)*Vec2(2, 1) == Vec2(4, 4));
	assert("divide", Vec2(4,2)/Vec2(2, 2)== Vec2(2, 1));
	assert("scale", Vec2(4,3)*2 == Vec2(8, 6));
	assert("assignment", (Vec2(1,1) = Vec2(2, 3)) == Vec2(2, 3));
	assert("mutable addition", (Vec2(1,1) += Vec2(2, 3)) == Vec2(3, 4));
	assert("mutable subtraction", (Vec2(4,3) -= Vec2(2, 1)) == Vec2(2, 2));
	assert("mutable multiply", (Vec2(2,4) *= Vec2(2, 1)) == Vec2(4, 4));
	assert("mutable divide", (Vec2(4,2) /= Vec2(2, 2)) == Vec2(2, 1));
	assert("mutable scale", (Vec2(4,3) *= 2) == Vec2(8, 6));
	assert("length", fabsf((Vec2(4,4)).length() - 5.65685) <= 0.00001);
	assert("length2", (Vec2(2,4)).length2() == 20);
	assert("dist", fabsf((Vec2(2,4)).dist(Vec2(3,5)) - 1.4142135) <= 0.000001);
	assert("dist2", (Vec2(2,4)).dist2(Vec2(3,5)) == 2);
	
	Vec2 normal = (Vec2(2,4)).normal();
	assert("normal", fabsf(normal.length() - 1.0) <= 0.00001 && normal.epsilonEquals(Vec2(0.4472, 0.89443), 0.0001));
	assert("dot", (Vec2(2,3)).dot(Vec2(4,1)) == 11);
	
	assert("angle", fabsf((Vec2(0,-1)).angle(Vec2(1,0))*(180/M_PI) - 90) < 0.0001);
	assert("angle2", fabsf((Vec2(1,1)).angle2(Vec2(1,0), Vec2(2,1))*(180/M_PI) - 90) < 0.0001);
	
	assert("rotate", (Vec2(2,0)).rotate(Vec2(1,0), M_PI/2).epsilonEquals(Vec2(1,1), 0.0001));
	assert("string", (string)Vec2(2,4) == "(2.000000, 4.000000)");
}

