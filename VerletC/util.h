#pragma once

float lerp(float a, float b, float p) {
	return (b-a)*p + a;
}

void drawCircle(Vec2 center, float radius) {
	glBegin(GL_POLYGON);
	for (int i=0; i<16; i++) {
		float a = i/16.0f*2.0f*M_PI;
		glVertex2f(center.x+cosf(a)*radius, center.y+sinf(a)*radius);
	}
	glEnd();
}

void drawLine(Vec2 a, Vec2 b) {
	glBegin(GL_LINES);
	{
		glVertex2f(a.x, a.y);
		glVertex2f(b.x, b.y);
		
	}glEnd();
}