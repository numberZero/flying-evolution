#pragma once
#include <atomic>
#include <list>
#include <thread>
#include "ship.hxx"

struct ProgramDesc
{
	Program *program;
	std::string name;
	BodyState start;
	BodyState finish;
};

struct Mediator
{
private:
	std::atomic<ProgramDesc const *> program;

public:
	bool give(ProgramDesc const *source);
	bool take(ProgramDesc const *& destination);
};

struct EvolutionProgramDesc:
	ProgramDesc
{
	EvolutionProgramDesc *base;
};

struct EvolutionThread
{
private:
	std::unique_ptr<std::thread> t;
	std::list<EvolutionProgramDesc*> programs;

	bool real_build_program(Program const *base, ProgramDesc *desc, bool use_hp = true);
	void build_program(EvolutionProgramDesc *desc);
	EvolutionProgramDesc *init_base_program(Float len);
	void init_program_list(Float len, Float side, Float step = 10.0);
	void execute();

public:
	Mediator m;
	void start();
};
