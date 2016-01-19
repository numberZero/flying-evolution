#pragma once
#include <list>
#include "simulation.hxx"

struct Model;

extern const BodyDesc& ship_desc;
extern const Model& ship_model;
extern const Float ship_mass;
extern const Float ship_full_acceleration;
extern const Float ship_engine_force;
extern const Float ship_engine_acceleration;

struct Ship: Body
{
	Float engine_left = 0.0;
	Float engine_right = 0.0;
	
	explicit Ship(Vector _position);
	void on_tick(Float duration) override;
};

struct Statement
{
	Float duration;
	Float engine_left;
	Float engine_right;
	
	Statement(Float _duration = 1.0, Float _engine_left = 0.0, Float _engine_right = 0.0) :
		duration(_duration),
		engine_left(_engine_left),
		engine_right(_engine_right)
	{
	}
};

struct Program
{
	typedef std::list<Statement> Code;
	typedef Code::const_iterator Iterator;
	Code code;
	Vector start;
	Vector target;
};

struct ProgrammedShip: Ship
{
	Program const& program;
	Program::Iterator current_statement;
	Float current_statement_shift;
	
	explicit ProgrammedShip(Vector _position, Program const& _program);
	void on_tick(Float duration) override;
	bool isRunning();
};
