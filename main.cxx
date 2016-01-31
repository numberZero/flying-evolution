#include <iostream>
#include <memory>
#include <GL/gl.h>
#include <SDL.h>
#include "simulation/evolution.hxx"
#include "common/model.hxx"
#include "simulation/ship.hxx"
#include "simulation/simulation.hxx"
#include "sound/sound.hxx"

SDL_Window *window;
static SDL_GLContext context;
static long t_base;

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

static ProgramDesc base_prg;
static ProgramDesc const *prg;
static std::unique_ptr<ProgrammedShip> ship;
static std::unique_ptr<Simulation> sim;
static EvolutionThread th;

void restartSimulation()
{
	sim.reset();
	ship.reset();
	th.m.take(prg);
	ship.reset(new ProgrammedShip(prg->start, *prg->program));
	sim.reset(new Simulation);
	sim->bodies.push_back(ship.get());
}

void init()
{
	base_prg.program = new Program();
	prg = &base_prg;
	restartSimulation();
	th.start();
}

void drawCross(Vector pos, Float r)
{
	glBegin(GL_LINES);
	glVertex2f(pos[0] - r, pos[1] - r);
	glVertex2f(pos[0] + r, pos[1] + r);
	glVertex2f(pos[0] - r, pos[1] + r);
	glVertex2f(pos[0] + r, pos[1] - r);
	glEnd();
}

void drawShip(Ship const& ship)
{
	static const Float base_x = 10.0;
	static const Float base_y = 7.0;
	static const Float size_x = 5.0;
	static const Float size_y = 20.0;
	Float x, y, sx, sy;
	glPushMatrix();
	glTranslated(ship.state.position[0], ship.state.position[1], 0.0);
	glRotated((180.0 / M_PI) * ship.state.rotation, 0.0, 0.0, 1.0);
	glColor4f(1.00, 1.00, 1.00, 1.00);
	ship_model.draw();
	glColor4f(1.00, 0.50, 0.00, 1.00);
	glBegin(GL_QUADS);
	
	x = -base_x;
	y = -std::copysign(base_y, ship.engine_left);
	sx = 0.5 * size_x * std::pow(std::abs(ship.engine_left), 0.25);
	sy = -0.5 * size_y * ship.engine_left;
	glVertex2f(x, y);
	glVertex2f(x + sx, y + sy);
	glVertex2f(x, y + 2.0 * sy);
	glVertex2f(x - sx, y + sy);
	
	x = base_x;
	y = -std::copysign(base_y, ship.engine_right);
	sx = 0.5 * size_x * std::pow(std::abs(ship.engine_right), 0.25);
	sy = -0.5 * size_y * ship.engine_right;
	glVertex2f(x, y);
	glVertex2f(x + sx, y + sy);
	glVertex2f(x, y + 2.0 * sy);
	glVertex2f(x - sx, y + sy);
	
	glEnd();
	glPopMatrix();
}

void step()
{
	static Float t = 0.0;
	static long n = 0;
	static Float fps = 0.0;
	static Float jitter = 0.0;
	static Float tjitter = 0.0;
	static Float mjitter = 0.0;
	static const Float step = 0.02;
	long t_now = SDL_GetTicks();
	double dt = (t_now - t_base) * 0.001;
	t_base = t_now;
	t += dt;
	++n;
	if(t >= 1.0)
	{
		fps = n / t;
		mjitter = tjitter / n;
		tjitter = 0;
		t = 0;
		n = 0;
	}
	tjitter += jitter;
	jitter -= dt;
	
	if(!ship->isRunning())
		restartSimulation();
	
	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	
	if(jitter < 0)
	{
		sim->on_before_tick();
		sim->on_tick(step);
		sim->on_after_tick();
		jitter += step;
		updateAudio(ship->engine_left, ship->engine_right, step);
	}
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	drawShip(*ship);
	
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
	
	glColor4f(1.0, 0.0, 0.0, 1.0);
	drawCross(prg->finish.position, 5.0);
	
	glColor4f(0.0, 1.0, 0.0, 0.5);
	textOut(-390.0, 270.0, 20.0, std::to_string(fps) + "\n" + std::to_string(mjitter));
	
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
	
	glEnable(GL_POLYGON_SMOOTH);
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
	initAudio();
	init();
	run();
	closeAudio();
	SDL_Quit();
	return 0;
}
