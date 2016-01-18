#include "ship.hxx"

const BodyDesc ship_desc{ship_mass, 200.0};
const Float ship_mass = 3000.0;
const Float ship_full_acceleration = 30.0;
const Float ship_engine_force = ship_mass * ship_engine_acceleration;
const Float ship_engine_acceleration = ship_full_acceleration / 2.0;

Ship::Ship(Vector _position) :
	Body(ship_desc)
{
	position = _position;
}

void Ship::on_before_tick()
{
	Body::on_before_tick();
	applyForceRel(Vector{0.0, ship_engine_force * engine_left}, Vector{-8.0, 0.0});
	applyForceRel(Vector{0.0, ship_engine_force * engine_right}, Vector{8.0, 0.0});
}
