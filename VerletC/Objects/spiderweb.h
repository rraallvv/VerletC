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

struct Spiderweb : public Composite {
	
	Spiderweb(VerletJS* sim, Vec2 origin, float radius, int segments, int depth) {
		
		float stiffness = 0.6;
		float tensor = 0.3;
		float stride = (2*M_PI)/segments;
		int n = segments*depth;
		float radiusStride = radius/n;
		int i, c;
		
		// particles
		for (i=0;i<n;++i) {
			float theta = i*stride + cosf(i*0.4)*0.05 + cosf(i*0.05)*0.2;
			float shrinkingRadius = radius - radiusStride*i + cosf(i*0.1)*20;
			
			float offy = cosf(theta*2.1)*(radius/depth)*0.2;
			particles.push_back(new Particle(Vec2(origin.x + cosf(theta)*shrinkingRadius, origin.y + sinf(theta)*shrinkingRadius + offy)));
		}
		
		for (i=0;i<segments;i+=4)
			pin(i);
		
		// constraints
		for (i=0;i<n-1;++i) {
			// neighbor
			constraints.push_back(new DistanceConstraint(particles[i], particles[i+1], stiffness));
			
			// span rings
			float off = i + segments;
			if (off < n-1)
				constraints.push_back(new DistanceConstraint(particles[i], particles[off], stiffness));
			else
				constraints.push_back(new DistanceConstraint(particles[i], particles[n-1], stiffness));
		}
		
		constraints.push_back(new DistanceConstraint(particles[0], particles[segments-1], stiffness));
		
		// strech the web
		for (c=0;c<constraints.size();c++)
			if (constraints[c]->type & Constraint::DISTANCE) {
				DistanceConstraint* constraint =  (DistanceConstraint*)constraints[c];
				constraint->distance *= tensor;
			}
		
		sim->composites.push_back(this);
	}
	
	void drawParticles() {
		int i;
		for (i=0; i<particles.size(); i++) {
			Particle* point = particles[i];
			
			glColor3ub(45, 173, 143);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			drawCircle(point->pos, 1.3);
		}
	}
};

struct Spider : public Composite {
	Particles legs;
	int legIndex = 0;
	
	Particle* head;
	Particle* thorax;
	Particle* abdomen;
	
	Spiderweb* spiderweb = NULL;
	
	Spider(VerletJS* sim, Spiderweb* spiderweb, Vec2 origin): spiderweb(spiderweb) {
		int i;
		float legSeg1Stiffness = 0.99;
		float legSeg2Stiffness = 0.99;
		float legSeg3Stiffness = 0.99;
		float legSeg4Stiffness = 0.99;
		
		float joint1Stiffness = 1;
		float joint2Stiffness = 0.4;
		float joint3Stiffness = 0.9;
		
		float bodyStiffness = 1;
		float bodyJointStiffness = 1;
		
		head = new Particle(origin+Vec2(0,-5));
		thorax = new Particle(origin);
		abdomen = new Particle(origin+Vec2(0,10));
		
		particles.push_back(thorax);
		particles.push_back(head);
		particles.push_back(abdomen);
		
		constraints.push_back(new DistanceConstraint(head, thorax, bodyStiffness));
		
		constraints.push_back(new DistanceConstraint(abdomen, thorax, bodyStiffness));
		constraints.push_back(new AngleConstraint(abdomen, thorax, head, 0.4));
		
		// legs
		for (i=0;i<4;++i) {
			particles.push_back(new Particle(particles[0]->pos+Vec2(3,(i-1.5)*3)));
			particles.push_back(new Particle(particles[0]->pos+Vec2(-3,(i-1.5)*3)));
			
			int len = (int)particles.size();
			
			constraints.push_back(new DistanceConstraint(particles[len-2], thorax, legSeg1Stiffness));
			constraints.push_back(new DistanceConstraint(particles[len-1], thorax, legSeg1Stiffness));
			
			
			float lenCoef = 1;
			if (i == 1 || i == 2)
				lenCoef = 0.7;
			else if (i == 3)
				lenCoef = 0.9;
			
			particles.push_back(new Particle(particles[len-2]->pos+(Vec2(20,(i-1.5)*30)).normal()*20*lenCoef));
			particles.push_back(new Particle(particles[len-1]->pos+(Vec2(-20,(i-1.5)*30)).normal()*20*lenCoef));
			
			len = (int)particles.size();
			constraints.push_back(new DistanceConstraint(particles[len-4], particles[len-2], legSeg2Stiffness));
			constraints.push_back(new DistanceConstraint(particles[len-3], particles[len-1], legSeg2Stiffness));
			
			particles.push_back(new Particle(particles[len-2]->pos+(Vec2(20,(i-1.5)*50)).normal()*20*lenCoef));
			particles.push_back(new Particle(particles[len-1]->pos+(Vec2(-20,(i-1.5)*50)).normal()*20*lenCoef));
			
			len = (int)particles.size();
			constraints.push_back(new DistanceConstraint(particles[len-4], particles[len-2], legSeg3Stiffness));
			constraints.push_back(new DistanceConstraint(particles[len-3], particles[len-1], legSeg3Stiffness));
			
			
			Particle* rightFoot = new Particle(particles[len-2]->pos+(Vec2(20,(i-1.5)*100)).normal()*12*lenCoef);
			Particle* leftFoot = new Particle(particles[len-1]->pos+(Vec2(-20,(i-1.5)*100)).normal()*12*lenCoef);
			particles.push_back(rightFoot);
			particles.push_back(leftFoot);
			
			legs.push_back(rightFoot);
			legs.push_back(leftFoot);
			
			len = (int)particles.size();
			constraints.push_back(new DistanceConstraint(particles[len-4], particles[len-2], legSeg4Stiffness));
			constraints.push_back(new DistanceConstraint(particles[len-3], particles[len-1], legSeg4Stiffness));
			
			
			constraints.push_back(new AngleConstraint(particles[len-6], particles[len-4], particles[len-2], joint3Stiffness));
			constraints.push_back(new AngleConstraint(particles[len-6+1], particles[len-4+1], particles[len-2+1], joint3Stiffness));
			
			constraints.push_back(new AngleConstraint(particles[len-8], particles[len-6], particles[len-4], joint2Stiffness));
			constraints.push_back(new AngleConstraint(particles[len-8+1], particles[len-6+1], particles[len-4+1], joint2Stiffness));
			
			constraints.push_back(new AngleConstraint(particles[0], particles[len-8], particles[len-6], joint1Stiffness));
			constraints.push_back(new AngleConstraint(particles[0], particles[len-8+1], particles[len-6+1], joint1Stiffness));
			
			constraints.push_back(new AngleConstraint(particles[1], particles[0], particles[len-8], bodyJointStiffness));
			constraints.push_back(new AngleConstraint(particles[1], particles[0], particles[len-8+1], bodyJointStiffness));
		}
		
		sim->composites.push_back(this);
	}
	
	void drawConstraints() {
		int i;
		
		glColor3ub(0, 0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		drawCircle(head->pos, 4);
		
		drawCircle(thorax->pos, 4);
		
		drawCircle(abdomen->pos, 8);
		
		for (i=3;i<constraints.size();++i) {
			if (constraints[i]->type & Constraint::DISTANCE) {
				DistanceConstraint* constraint = (DistanceConstraint*)constraints[i];
				glLineWidth(1.0);
				drawLine(constraint->a->pos, constraint->b->pos);
				
				// draw legs
				if (
					(i >= 2 && i <= 4)
					|| (i >= (2*9)+1 && i <= (2*9)+2)
					|| (i >= (2*17)+1 && i <= (2*17)+2)
					|| (i >= (2*25)+1 && i <= (2*25)+2)
					) {
					glLineWidth(3);
					drawLine(constraint->a->pos, constraint->b->pos);
				} else if (
						   (i >= 4 && i <= 6)
						   || (i >= (2*9)+3 && i <= (2*9)+4)
						   || (i >= (2*17)+3 && i <= (2*17)+4)
						   || (i >= (2*25)+3 && i <= (2*25)+4)
						   ) {
					glLineWidth(2);
					drawLine(constraint->a->pos, constraint->b->pos);
				} else if (
						   (i >= 6 && i <= 8)
						   || (i >= (2*9)+5 && i <= (2*9)+6)
						   || (i >= (2*17)+5 && i <= (2*17)+6)
						   || (i >= (2*25)+5 && i <= (2*25)+6)
						   ) {
					glLineWidth(1.5);
					drawLine(constraint->a->pos, constraint->b->pos);
				} else {
				}
			}
		}
	}
	
	void drawParticles() {
	}
	
	void crawl(int leg) {
		if (!spiderweb)
			return;
		
		float stepRadius = 100;
		float minStepRadius = 35;
		
		float theta = particles[0]->pos.angle2(particles[0]->pos+Vec2(1,0), particles[1]->pos);
		
		Vec2 boundry1 = Vec2(cosf(theta), sinf(theta));
		Vec2 boundry2 = Vec2(cosf(theta+M_PI/2), sinf(theta+M_PI/2));
		
		
		float flag1 = leg < 4 ? 1 : -1;
		float flag2 = leg%2 == 0 ? 1 : 0;
		
		Particles paths;
		
		int i;
		for (i=0; i<spiderweb->particles.size(); i++) {
			if (
				(spiderweb->particles[i]->pos-particles[0]->pos).dot(boundry1)*flag1 >= 0
				&& (spiderweb->particles[i]->pos-particles[0]->pos).dot(boundry2)*flag2 >= 0
				) {
				float d2 = spiderweb->particles[i]->pos.dist2(particles[0]->pos);
				
				if (!(d2 >= minStepRadius*minStepRadius && d2 <= stepRadius*stepRadius))
					continue;
				
				bool leftFoot = false;
				int j;
				for (j=0; j<constraints.size(); j++) {
					int k;
					for (k=0;k<8;++k) {
						if (constraints[j]->type & Constraint::DISTANCE) {
							DistanceConstraint* constraint = (DistanceConstraint*)constraints[j];
							if (constraint->a == legs[k] && constraint->b == spiderweb->particles[i])
								leftFoot = true;
						}
					}
				}
				
				if (!leftFoot)
					paths.push_back(spiderweb->particles[i]);
			}
		}
		
		for(Constraints::iterator it=constraints.begin(); it!=constraints.end(); it++) {
			if (((Constraint*)*it)->type & Constraint::DISTANCE) {
				DistanceConstraint* constraint = (DistanceConstraint*)*it;
				if (constraint->a == legs[leg]) {
					constraints.erase(it);
					break;
				}
			}
		}
		
		if (paths.size() > 0) {
			random_shuffle(paths.begin(), paths.end());
			constraints.push_back(new DistanceConstraint(legs[leg], paths[0], 1, 0));
		}
	}
	
	void update(float dt) {
		// animation loop
		if (floor(random()%4) == 0) {
			crawl(((legIndex++)*3)%8);
		}
	}
};
