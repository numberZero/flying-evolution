#include "evolution.hxx"
#include <functional>
#include <iostream>
#include <random>

#define SYNCHRONIZED	Lock _328514_guard(mtx)

bool Mediator::give(PProgram&& source)
{
	SYNCHRONIZED;
	bool result = !program;
	program = std::move(source);
	return result;
}

bool Mediator::take(PProgram& destination)
{
	SYNCHRONIZED;
	if(program)
	{
		destination = std::move(program);
		return true;
	}
	return false;
}

void change_program(Program* prog)
{
	static std::ranlux24 gen(std::time(nullptr));
	static std::discrete_distribution<> rnd_mode{7, 3, 90};
	static std::lognormal_distribution<Float> time_base{-1.0, 0.1};
	static std::normal_distribution<Float> eng_base{0.0, 0.1};
	static std::discrete_distribution<> rnd_change{1, 1, 4, 4};
	static std::lognormal_distribution<Float> time_shift{0.0, 0.1};
	static std::normal_distribution<Float> eng_shift{0.0, 0.02};
	static std::bernoulli_distribution d5050;
	int mode = rnd_mode(gen);
	switch(mode)
	{
		case 2:
		{
			int change = rnd_change(gen);
			std::uniform_int_distribution<> pos(0, prog->code.size() - 1);
			auto obj = std::next(prog->code.begin(), pos(gen));
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
			break;
		}
		case 1:
		{
			std::uniform_int_distribution<> pos(0, prog->code.size() - 1);
			auto obj = std::next(prog->code.begin(), pos(gen));
			prog->code.erase(obj);
		}
		case 0:
		{
			std::uniform_int_distribution<> pos(0, prog->code.size());
			auto obj = std::next(prog->code.begin(), pos(gen));
			prog->code.emplace(obj, time_base(gen), eng_base(gen), eng_base(gen));
		}
	}
}

std::pair<Float, Float> eval_program(Program* prog, Float dt = 0.01)
{
	Simulation sim;
	ProgrammedShip ship(prog->start, *prog);
	Float q = 0;
	Float t = 0;
	sim.bodies.push_back(&ship);
	while(ship.isRunning())
	{
		sim.on_before_tick();
		sim.on_tick(dt);
		sim.on_after_tick();
		Float q2 = 100.0 / (ship.position - prog->target).squaredNorm();
		if(q2 > q)
		{
			q = q2;
			t = sim.global_time;
		}
	}
	return{q, t};
// 	return 100.0 / (ship.position - prog->target).squaredNorm();
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

void EvolutionThread::execute()
{
	static const int pcount = 8;
	std::cout << "Starting evolution thread" << std::endl;
	std::cout << "Initializing base program" << std::endl;
	p.reset(new Program());
	p->start = Vector(0.0, -200.0);
	p->target = Vector(0.0, 200.0);
	p->code.emplace_back(0.5, 0.0, 0.0);
	p->code.emplace_back(std::sqrt(800.0 / ship_full_acceleration), 1.0, 1.0);
	p->code.emplace_back(0.5, 0.0, 0.0);
	std::cout << "Evolution!" << std::endl;
	Float dt = 0.001;
	Float minq = 1.0;
	bool fin = false;
	for(;;)
	{
		std::cout << "Evolution step started" << std::endl;
		PProgram ps[pcount];
		for(int k = 0; k != pcount; ++k)
			ps[k].reset(new Program(*p));
		m.give(std::move(p));
		for(int k = 1; k != pcount; ++k)
			change_program(ps[k].get());
		int id = 0;
		Float q = 0;
		Float t = 0;
		for(int k = 0; k != pcount; ++k)
		{
			auto q2 = eval_program(ps[k].get(), dt);
			if(q2.first > q)
			{
				q = q2.first;
				t = q2.second;
				id = k;
			}
		}
		p = std::move(ps[id]);
		clampProgram(p.get(), t);
		std::cout << "Evolution step finished: " << q  << std::endl;
		if(q > minq)
		{
			std::cout << "Local goal reached" << std::endl;
			Vector shift = target - p->target;
			Float len = shift.norm();
			if(fin)
			{
				std::cout << "Evolution finished succesfully!" << std::endl;
				return;
			}
			if(len < 10.0)
			{
				if(!fin)
					std::cout << "Switching to the high-precision mode" << std::endl;
				p->target = target;
				minq = 10.0;
				dt = 0.001;
				fin = true;
			}
			else
			{
				p->target += (10.0 / len) * shift;
			}
		}
	}
}

void EvolutionThread::start()
{
	if(t)
		return;
	target = Vector(300.0, -200.0);
	t.reset(new std::thread(&EvolutionThread::execute, this));
	t->detach();
}
