#pragma once
#include <memory>
#include <mutex>
#include <thread>
#include "ship.hxx"

typedef std::unique_ptr<Program> PProgram;

struct Mediator
{
private:
	typedef std::mutex Mutex;
	typedef std::lock_guard<Mutex> Lock;
	Mutex mtx;
	PProgram program;

public:
	bool give(PProgram&& source);
	bool take(PProgram& destination);
};

struct EvolutionThread
{
private:
	std::unique_ptr<std::thread> t;
	PProgram p;
	Vector target;

	void execute();

public:
	Mediator m;
	void start();
};
