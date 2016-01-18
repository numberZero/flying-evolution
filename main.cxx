#include <iostream>
#include "GL/gl.h"
#include "SDL.h"
#include "ship.hxx"
#include "simulation.hxx"

SDL_Window *window;
SDL_GLContext context;
long t_base;

bool events()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				return false;
		}
	}
	return true;
}

Ship ship1{Vector{-50.0, -100.0}};
Ship ship2{Vector{-50.0, 100.0}};
Simulation sim;

void init()
{
	ship1.engine_left = 1.0;
	ship1.engine_right = 1.0;
	ship2.engine_left = 1.0;
	ship2.engine_right = 0.0;
	sim.bodies.push_back(&ship1);
	sim.bodies.push_back(&ship2);
}

void drawShip(Body const& body)
{
	glPushMatrix();
	glTranslated(body.position[0], body.position[1], 0.0);
	glRotated(body.rotation * (M_PI / 180.0), 0.0, 0.0, 1.0);
	
	glBegin(GL_LINE_LOOP);
	glVertex2f(-10.0, -7.0);
	glVertex2f(10.0, 7.0);
	glVertex2f(10.0, -7.0);
	glVertex2f(-10.0, 7.0);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex2f(-4.0, 11.0);
	glVertex2f(0.0, 15.0);
	glVertex2f(4.0, 11.0);
	glVertex2f(0.0, 15.0);
	glVertex2f(0.0, -15.0);
	glVertex2f(0.0, 15.0);
	glEnd();
	
	glPopMatrix();
}

void step()
{
	long t_now = SDL_GetTicks();
	double dt = (t_now - t_base) * 0.001;
	t_base = t_now;
	
	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	
	sim.on_before_tick();
	sim.on_tick(dt);
	sim.on_after_tick();
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	drawShip(ship1);
	drawShip(ship2);
	
	glColor4f(1.0, 1.0, 0.0, 0.2);
	glBegin(GL_LINES);
	for(long i = -4; i <= 4; ++i)
	{
		glVertex2f(100.0 * i, -300.0);
		glVertex2f(100.0 * i, 300.0);
	}
	for(long j = -3; j <= 3; ++j)
	{
		glVertex2f(-400, 100.0 * j);
		glVertex2f(400, 100.0 * j);
	}
	glEnd();
	
	glFlush();
	glFinish();
	SDL_GL_SwapWindow(window);
	
}

void run()
{
	while(events())
		step();
}

void initSDL()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Subsimulation test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);
	t_base = SDL_GetTicks();
}

void initGL()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH);
	
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.5);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-400, 400, -300, 300, -10, 100);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
	initSDL();
	initGL();
	init();
	run();
	SDL_Quit();
	return 0;
}
