#include <iostream>
#include <GLUT/GLUT.h>
#include <OpenGL/OpenGL.h>

#include "util.h"
#include "demo.h"

//////////////////////
// simulation metrics

int sim_w = 800;
int sim_h = 500;

float sim_min_scale;
float sim_scale_w;
float sim_scale_h;
float sim_x_origin;
float sim_y_origin;


//////////////////////
// main program

void idle() {
	glutPostRedisplay();
}

void display() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(sim_x_origin, sim_y_origin, 0);
	glScalef(sim_min_scale * sim_scale_w, -sim_min_scale * sim_scale_h, 1);
	
	float dt = time_interval();
	
	if (demo::active_demo != 2)
		glClearColor(1, 1, 1, 1);
	else
		glClearColor(0, 0, 0, 1);
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	demo::sim->update(dt);
	demo::sim->draw();
	
	if (demo::active_demo != 2)
		glColor3f(0, 0, 0);
	else
		glColor3f(1, 1, 1);
	
	glLoadIdentity();
	glTranslatef(0, sim_h, 0);
	glScalef(1, -1, 1);
	
	float lw = 5.0f*sim_scale_w;
	float lh = 15.0f*sim_scale_h;
	
	if (demo::show_help) {
		int l=0;
		
		glRasterPos2d(lw,++l*lh);
		draw_str("Keys:", GLUT_BITMAP_HELVETICA_12);
		glRasterPos2d(lw,++l*lh);
		draw_str(" [ Left / Right ] - previous / next simulation.", GLUT_BITMAP_HELVETICA_12, 37, 38);
		glRasterPos2d(lw,++l*lh);
		draw_str(" [ R ] - restart current simulation.", GLUT_BITMAP_HELVETICA_12);
		glRasterPos2d(lw,++l*lh);
		draw_str(" [ H ] - show / hide help.", GLUT_BITMAP_HELVETICA_12);
		glRasterPos2d(lw,++l*lh);
		draw_str(" [ ESC ] - quit.", GLUT_BITMAP_HELVETICA_12);
		++l;
		glRasterPos2d(lw,++l*lh);
		draw_str("Hold the mouse button to drag point.", GLUT_BITMAP_HELVETICA_12);
		glRasterPos2d(lw,++l*lh);
	}
	
	glRasterPos2d(lw,sim_h-0.3f*lh);
	draw_str("Framerate %.2f", GLUT_BITMAP_HELVETICA_12, 1.0f/dt);
	
	glutSwapBuffers();
}

void reshape(int width, int height) {
	glutReshapeWindow(max(width, sim_w/2), max(height, sim_h/2));
	
	glViewport( 0, 0, width, height );
	
	sim_scale_w = sim_w/(float)width;
	sim_scale_h = sim_h/(float)height;
	
	sim_min_scale = 1.0f/max(sim_scale_w, sim_scale_h);
	
	sim_y_origin = sim_h - 0.5 * sim_h * ( 1.0 - sim_min_scale * sim_scale_h );
	sim_x_origin = 0.5 * sim_w * ( 1.0 - sim_min_scale * sim_scale_w );
}

void mouse ( int button, int state, int x, int y ) {
	x = (x * sim_scale_w - 0.5 * sim_w)/(sim_min_scale * sim_scale_w) + 0.5 * sim_w;
	y = (y * sim_scale_h - 0.5 * sim_h)/(sim_min_scale * sim_scale_h) + 0.5 * sim_h;
	
	demo::sim->onMouseClick(button, state, x, y);
}

void motion ( int x, int y ) {
	x = (x * sim_scale_w - 0.5 * sim_w)/(sim_min_scale * sim_scale_w) + 0.5 * sim_w;
	y = (y * sim_scale_h - 0.5 * sim_h)/(sim_min_scale * sim_scale_h) + 0.5 * sim_h;
	
	demo::sim->onMouseMove(x, y);
}

void specialkeys(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_LEFT:
			demo::switch_demo(-1);
			break;
			
		case GLUT_KEY_RIGHT:
			demo::switch_demo(1);
			break;
			
		default :
			break;
	}
}

void keyboard( unsigned char key, int x, int y ) {
	key = toupper(key);
	switch (key) {
		case 27:
			exit(0);
			break;
			
		case 'R':
			demo::switch_demo(0);
			break;
			
		case 'H':
			demo::show_help = !demo::show_help;
			break;
			
		default :
			break;
	}
}

int main(int argc, char * argv[]) {
	
	glutInit(&argc, argv);
	glutInitWindowSize ( sim_w, sim_h );
	glutCreateWindow("VerletC Demo");
	
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkeys);
	
	glutMouseFunc(mouse);
	glutMotionFunc (motion);
	glutPassiveMotionFunc(motion);
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, sim_w, 0, sim_h, -1, 1);
	
	demo::init(sim_w, sim_h);
	
	glutMainLoop();
	
	return 0;
}
