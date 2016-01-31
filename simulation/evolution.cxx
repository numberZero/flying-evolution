#include "evolution.hxx"
#include <cmath>
#include <functional>
#include <iostream>
#include <random>

#define SYNCHRONIZED	Lock _328514_guard(mtx)

bool Mediator::give(ProgramDesc const *source)
{
	program.store(source);
}

bool Mediator::take(ProgramDesc const *& destination)
{
	ProgramDesc const *p = program.load();
	if(p)
	{
		destination = p;
		return true;
	}
	return false;
}

static std::ranlux24 gen(std::time(nullptr));
static std::bernoulli_distribution d5050;
static std::uniform_real_distribution<Float> duni{0.0, 1.0};

void change_statement(Statement* obj)
{
	static std::discrete_distribution<> rnd_change{1, 1, 4, 4};
	static std::lognormal_distribution<Float> time_shift{0.0, 0.1};
	static std::normal_distribution<Float> eng_shift{0.0, 0.02};
	int change = rnd_change(gen);
	Float shift;
	if(change)
		shift = eng_shift(gen);
	else
		shift = time_shift(gen);
	switch(change)
	{
		case 0: // time
			obj->duration *= shift;
			break;
		case 1: // one engine
			if(d5050(gen))
				obj->engine_left += shift;
			else
				obj->engine_right += shift;
			break;
		case 2: // both engines, lin.
			obj->engine_left += shift;
			obj->engine_right += shift;
			break;
		case 3: // both engines, rot.
			obj->engine_left += shift;
			obj->engine_right -= shift;
			break;
	}
}

void change_program(Program* prog, bool allow_split = false)
{
	static std::discrete_distribution<> rnd_mode{85, 7, 3, 5};
	static std::discrete_distribution<> rnd_mode_ns{90, 7, 3};
	static std::lognormal_distribution<Float> time_base{-1.0, 0.1};
	static std::normal_distribution<Float> eng_base{0.0, 0.1};
	std::uniform_int_distribution<> rnd_pos(0, prog->code.size() - 1);
	int mode = (allow_split ? rnd_mode : rnd_mode_ns)(gen);
	switch(mode)
	{
		case 1:
			rnd_pos = std::uniform_int_distribution<> (0, prog->code.size());
			break;
	}
	auto pobj = std::next(prog->code.begin(), rnd_pos(gen));
	switch(mode)
	{
		case 0:
			change_statement(&*pobj);
			break;
		case 1:
			prog->code.emplace(pobj, time_base(gen), eng_base(gen), eng_base(gen));
			break;
		case 2:
			prog->code.erase(pobj);
			break;
		case 3:
		{
			auto pobj2 = prog->code.emplace(pobj, *pobj);
			Float c = duni(gen);
			pobj2->duration *= c;
			pobj->duration *= 1 - c;
			if(d5050(gen))
				change_statement(&*pobj);
			else
				change_statement(&*pobj2);
			break;
		}
	}
}

std::pair<Float, Float> eval_program(ProgramDesc const *desc, Float dt = 0.01)
{
	Simulation sim;
	ProgrammedShip ship(desc->start, *desc->program);
	Float q = 0;
	Float t = 0;
#ifdef PENALTY_ROTATION
	Float r0 = 0;
#endif
	sim.bodies.push_back(&ship);
	while(ship.isRunning())
	{
		sim.on_before_tick();
		sim.on_tick(dt);
		sim.on_after_tick();
		if(std::abs(ship.state.angular_veloctiy) > 2.0) // ≈ 120°/s (1 rotation per 3 seconds)
			break; // too high angular velocity at this point

		Float b_total = 0.0;
		Float q_total = 100.0;

		Float b_pos = (ship.state.position - desc->finish.position).squaredNorm();
		b_total += b_pos;

#ifdef CHECK_VELOCITY
		Float b_vel =  0.1 * (ship.state.velocity - desc->finish.velocity).squaredNorm();
		b_total += b_vel;
#endif
#ifdef CHECK_ROTATION
		Float b_rot = 5.0 * (1.0 - std::cos(ship.state.rotation - desc->finish.rotation));
		b_total += b_rot * b_rot;
#endif
#ifdef CHECK_RVEL
		Float b_rvel = 1.0 * std::abs(ship.state.angular_veloctiy);
		b_total += b_rvel * b_rvel;
#endif
#ifdef CHECK_TIME
		Float b_time = 0.1 * sim.global_time;
		b_total += b_time;
#endif
#ifdef PENALTY_ROTATION
		r0 += dt * std::abs(ship.state.angular_veloctiy);
		Float p_rot = std::exp(-0.2 * r0 / (2.0 * M_PI));
		q_total *= p_rot;
#endif

		q_total /= b_total;
		if(q_total > q)
		{
			q = q_total;
			t = sim.global_time;
		}
	}
	return{q, t};
}

void clampProgram(Program* prog, Float t)
{
	Float gt = 0;
	auto i = prog->code.begin();
	for(; i != prog->code.end(); ++i)
	{
		Float gt2 = gt + i->duration;
		if(gt2 > t)
		{
			i->duration = t - gt;
			++i;
			break;
		}
		gt = gt2;
	}
	prog->code.erase(i, prog->code.end());
}

bool EvolutionThread::real_build_program(Program const *base, ProgramDesc *desc, bool use_hp)
{
	if(desc->program)
		throw std::logic_error("Program already built: " + desc->name);
	std::cout << "Evolution!" << std::endl;
	static const int pcount = 8;
	Float dt = 0.01;
	Float minq = 1.0;
	Float q = 0;
	long n = 100;
	std::unique_ptr<Program> current;
	while(q < minq)
	{
		std::unique_ptr<Program> ps[pcount];
		for(int k = 0; k != pcount; ++k)
			ps[k].reset(new Program(*base));
		for(int k = 1; k != pcount; ++k)
			change_program(ps[k].get());
		int id = 0;
		Float t = 0;
		q = 0;
		for(int k = 0; k != pcount; ++k)
		{
			ProgramDesc mydesc(*desc);
			mydesc.program = ps[k].get();
			auto q2 = eval_program(&mydesc, dt);
			if(q2.first > q)
			{
				q = q2.first;
				t = q2.second;
				id = k;
			}
		}
		current = std::move(ps[id]);
		clampProgram(current.get(), t);
		base = current.get();
		std::cout << "Evolution step finished: " << q  << std::endl;
		if(use_hp && (q > minq))
		{
			std::cout << "Switching to the high-precision mode" << std::endl;
			use_hp = false;
			minq = 10.0;
		}
		if(--n <= 0)
			return false;
	}
	std::cout << "Program built with quality: " << q  << std::endl;
	desc->program = current.release();
	return true;
}

void EvolutionThread::build_program(EvolutionProgramDesc *desc)
{
	if(desc->program)
		throw std::logic_error("Program already built: " + desc->name);
	if(!desc->base)
		throw std::logic_error("Can't build a program without a base: " + desc->name);
	if(!desc->base->program)
		build_program(desc->base);
	while(!real_build_program(desc->base->program, desc))
		std::cout << "Evolution failed for " << desc->name << ", restarting" << std::endl;
	m.give(desc);
}

EvolutionProgramDesc *EvolutionThread::init_base_program(Float len)
{
	std::cout << "Initializing base program" << std::endl;
	EvolutionProgramDesc *desc = new EvolutionProgramDesc();
	desc->base = nullptr;
	desc->name = "base";
	desc->start.position = Vector(0.0, 0.0);
	desc->finish.position = Vector(0.0, len);
	desc->program = new Program();
	desc->program->code.emplace_back(0.5, 0.0, 0.0);
	desc->program->code.emplace_back(std::sqrt(len / ship_full_acceleration), 1.0, 1.0);
	desc->program->code.emplace_back(std::sqrt(len / ship_full_acceleration), -1.0, -1.0);
	desc->program->code.emplace_back(0.5, 0.0, 0.0);
	return desc;
}

void EvolutionThread::init_program_list(Float len, Float side, Float step)
{
	EvolutionProgramDesc **table;
	EvolutionProgramDesc *base = init_base_program(len);
	std::cout << "Initializing program table" << std::endl;
	programs.push_back(base);
	long nx = std::ceil(side / step);
	long ny = std::ceil(len / step);
	Float dx = side / nx;
	Float dy = len / ny;
	++nx;
	++ny;
	table = new EvolutionProgramDesc * [nx * ny];
	table[nx * (ny - 1)] = base;
	for(long j = 0; j != ny; ++j)
		for(long i = 0; i != nx; ++i)
		{
			long k = i + nx * j;
			if((j != ny - 1) || (i != 0))
				table[k] = new EvolutionProgramDesc();
			table[k]->name += "(" + std::to_string(i) + ", " + std::to_string(j) + ")";
		}
	for(long j = 0; j != ny; ++j)
		for(long i = 0; i != nx; ++i)
		{
			long k = i + nx * j;
			long i2 = 2 * i >= j ? i + 1 : i;
			long j2 = 2 * j >= i ? j + 1 : j;
			if((i2 >= nx) || (j2 >= ny))
			{
				if(j == ny - 1)
				{
					if(i == 0)
						continue;
					i2 = i - 1;
					j2 = j;
				}
				else if(i == nx - 1)
				{
					i2 = i;
					j2 = j + 1;
				}
				else
					throw std::logic_error("Invalid table position");
			}
// 			std::cout << "(" << i << ", " << j << ") ← (" << i2 << ", " << j2 << ")" << std::endl;
			long k0 = i2 + nx * j2;
			table[k]->base = table[k0];
			table[k]->program = nullptr;
			table[k]->start = base->start;
			table[k]->finish.position = Vector(i * dx, j * dy);
			programs.push_back(table[k]);
		}
	delete[] table;
}

void EvolutionThread::execute()
{
	std::cout << "Starting evolution thread" << std::endl;
	init_program_list(200.0, 300.0);
	std::cout << "Running evolution" << std::endl;
	for(EvolutionProgramDesc* p: programs)
		if(!p->program)
			build_program(p);
	std::cout << "All programs built (" << programs.size() << " total)" << std::endl;
}

void EvolutionThread::start()
{
	if(t)
		return;
	t.reset(new std::thread(&EvolutionThread::execute, this));
	t->detach();
}
