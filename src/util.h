#pragma once

#include <GLUT/GLUT.h>
#include <sys/time.h>

static double seconds() {
	struct timeval t;
	gettimeofday(&t, NULL);
	unsigned long msec = t.tv_sec*1e6 + t.tv_usec;
	return msec/1.0e6;
}

static double time_interval() {
	static double psec = seconds();
	double csec = seconds();
	double sec = csec - psec;
	psec = csec;
	return sec;
}

void draw_str(const char* str, void *font, ...) {
	va_list args;
	char buffer[512];
	va_start(args,font);
	vsprintf(buffer,str,args);
	va_end(args);
	
	char *c = buffer;
	
	while (*c) glutBitmapCharacter(font, *c++);
}
