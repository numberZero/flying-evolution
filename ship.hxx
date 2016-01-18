#pragma once
#include "simulation.hxx"

extern const BodyDesc ship_desc;
extern const Float ship_mass;
extern const Float ship_full_acceleration;
extern const Float ship_engine_force;
extern const Float ship_engine_acceleration;

struct Ship: Body
{
	Float engine_left = 0.0;
	Float engine_right = 0.0;
	
	explicit Ship(Vector _position);
	virtual void on_before_tick();
};
