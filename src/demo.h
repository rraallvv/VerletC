#include "verlet.h"
#include "objects.h"
#include "tree.h"
#include "cloth.h"
#include "spiderweb.h"

namespace demo {
	
	VerletJS* sim = NULL;
	
	int sim_w = 800;
	int sim_h = 500;
	
	bool show_help = true;
	
	void switch_demo(int count);
	
	void demo_shapes();
	void demo_trees();
	void demo_cloth();
	void demo_spider();
	
	void (*demos[])() = {demo_shapes, demo_trees, demo_cloth, demo_spider};
	int num_demos = sizeof(demos)/sizeof(void*);
	int active_demo = 3;
	
	//////////////////////////////
	// initialise default demo
	
	void init(int width, int height) {
		switch_demo(0);
	}
	
	//////////////////////////////////////////////////////////
	// switch the current demo
	// count: +1 next, -1 previous, 0 reset
	
	void switch_demo(int count) {
		if (sim)
			delete sim;
		
		sim = new VerletJS(sim_w, sim_h);
		
		active_demo += max(-1,min(+1, count));
		
		if (active_demo >= num_demos)
			active_demo = active_demo % num_demos;
		else if (active_demo < 0)
			active_demo = num_demos - abs(active_demo) % num_demos;
		
		demos[active_demo]();
	}
	
	///////////////////
	// available demos
	
	void demo_shapes() {
		// settings
		sim->friction = 1;
		
		// entities
		Vec2 vertices[] = {Vec2(20,10), Vec2(40,10), Vec2(60,10), Vec2(80,10), Vec2(100,10)};
		Composite* segment = new LineSegments(sim, vertices, 0.02);
		segment->pin(0);
		segment->pin(4);
		
		Tire* tire1 = new Tire(sim, Vec2(200,50), 50, 30, 0.3, 0.9);
		Tire* tire2 = new Tire(sim, Vec2(400,50), 70, 7, 0.1, 0.2);
		Tire* tire3 = new Tire(sim, Vec2(600,50), 70, 3, 1, 1);
	}
	
	void demo_trees() {
		// settings
		sim->gravity = Vec2(0,0);
		sim->friction = 0.98;
		
		// entities
		Tree* tree1 = new Tree(sim, Vec2(sim_w/4,sim_h-120), 5, 70, 0.95, (M_PI/2)/3);
		tree1->debugDraw = true;
		
		Tree* tree2 = new Tree(sim, Vec2(sim_w - sim_w/4,sim_h-120), 5, 70, 0.95, (M_PI/2)/3);
	}
	
	void demo_cloth() {
		// settings
		sim->friction = 1;
		sim->highlightColor[0] = 255;
		sim->highlightColor[1] = 255;
		sim->highlightColor[2] = 255;
		
		// entities
		float min = fmin(sim_w,sim_h)*0.5;
		int segments = 20;
		
		Cloth* cloth = new Cloth(sim, Vec2(sim_w/2,sim_h/3), min, min, segments, 6, 0.9);
	}
	
	void demo_spider() {
		// settings
		
		// entities
		Spiderweb* spiderweb = new Spiderweb(sim, Vec2(sim_w/2,sim_h/2), fmin(sim_w, sim_h)/2, 20, 7);
		Spider* spider = new Spider(sim, spiderweb, Vec2(sim_w/2,-300));
	}
}
