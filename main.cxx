#include <iostream>
#include "GL/gl.h"
#include "SDL.h"
#include "ship.hxx"
#include "simulation.hxx"
#include "model.hxx"

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

void textOut(Float x, Float y, Float size, std::string text)
{
	Float scale = size / 2.5;
	glTranslated(x, y, 0);
	glScaled(scale, scale, scale);
	Float u = 0;
	Float v = 0;
	glBegin(GL_LINES);
	for(char c: text)
		switch(c)
		{
			case '.':
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 0.00, v + 0.50);
				u += 0.5;
				break;
			case '0':
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 2.00);
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 1.00, v + 2.00);
				u += 1.5;
				break;
			case '1':
				glVertex2d(u + 0.50, v + 1.50);
				glVertex2d(u + 1.00, v + 2.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 1.00, v + 2.00);
				u += 1.5;
				break;
			case '2':
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 0.00, v + 1.00);
				glVertex2d(u + 1.00, v + 1.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 2.00);
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 0.00, v + 1.00);
				glVertex2d(u + 1.00, v + 1.00);
				glVertex2d(u + 1.00, v + 2.00);
				u += 1.5;
				break;
			case '3':
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 0.00, v + 1.00);
				glVertex2d(u + 1.00, v + 1.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 2.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 1.00, v + 2.00);
				u += 1.5;
				break;
			case '4':
				glVertex2d(u + 0.00, v + 1.00);
				glVertex2d(u + 1.00, v + 1.00);
				glVertex2d(u + 0.00, v + 1.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 1.00, v + 2.00);
				u += 1.5;
				break;
			case '5':
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 0.00, v + 1.00);
				glVertex2d(u + 1.00, v + 1.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 2.00);
				glVertex2d(u + 0.00, v + 1.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 1.00, v + 1.00);
				u += 1.5;
				break;
			case '6':
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 0.00, v + 1.00);
				glVertex2d(u + 1.00, v + 1.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 2.00);
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 1.00, v + 1.00);
				u += 1.5;
				break;
			case '7':
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 2.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 1.00, v + 2.00);
				u += 1.5;
				break;
			case '8':
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 0.00, v + 1.00);
				glVertex2d(u + 1.00, v + 1.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 2.00);
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 1.00, v + 2.00);
				u += 1.5;
				break;
			case '9':
				glVertex2d(u + 0.00, v + 0.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 0.00, v + 1.00);
				glVertex2d(u + 1.00, v + 1.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 2.00);
				glVertex2d(u + 0.00, v + 1.00);
				glVertex2d(u + 0.00, v + 2.00);
				glVertex2d(u + 1.00, v + 0.00);
				glVertex2d(u + 1.00, v + 2.00);
				u += 1.5;
				break;
			case '\n':
				u = 0;
				v -= 3.0;
				break;
		}
	glEnd();
}

Program prg1;
ProgrammedShip ship1{Vector{-50.0, -100.0}, prg1};
Ship ship2{Vector{-50.0, 100.0}};
Simulation sim;

void init()
{
// 	prg1.code.emplace_back(0.5, 0.4, 0.0);
	prg1.code.emplace_back(0.5, 0.2, -0.2);
	prg1.code.emplace_back(0.5, -0.2, 0.2);
	prg1.code.emplace_back(5.0, 0.4, 0.4);
	ship1.current_statement = prg1.code.begin();
	ship1.engine_left = 1.0;
	ship1.engine_right = 1.0;
	ship2.engine_left = 1.0;
	ship2.engine_right = 0.0;
	sim.bodies.push_back(&ship1);
	sim.bodies.push_back(&ship2);
}

void drawShip(Body const& body)
{
	ship_model.draw(body.position, body.rotation);
}

void step()
{
	static Float t = 0.0;
	static long n = 0;
	static Float fps = 0.0;
	long t_now = SDL_GetTicks();
	double dt = (t_now - t_base) * 0.001;
	t += dt;
	++n;
	if(t >= 1.0)
	{
		fps = n / t;
		t = 0;
		n = 0;
	}
	clampIt(dt, 0.02);
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
	
	textOut(-390.0, 270.0, 20.0, std::to_string(fps));
	
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
