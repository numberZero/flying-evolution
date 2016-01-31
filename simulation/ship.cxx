#include "ship.hxx"
#include <cassert>
#include <iostream>
#include "model.hxx"

static struct ShipModelBuilder
{
	BodyDesc desc;
	Model model;

	ShipModelBuilder()
	{
		ModelBuilder bld;
		ModelBuilder bld2;
		Float m, rho, r;

		std::cout << "Subtotal: " << bld.getMass() << " kg" << std::endl;

		bld2.line(60.0, Vector{-10.00, -7.00}, Vector{-10.0, 7.00});
		bld2.line(60.0, Vector{10.00, -7.00}, Vector{10.0, 7.00});
		bld.merge(bld2, -bld2.getCenter());
		std::cout << "Engines: " << bld2.getMass() << " kg" << std::endl;
		std::cout << "Subtotal: " << bld.getMass() << " kg" << std::endl;
		bld2.clear();

		bld2.line(9.0, Vector{0.00, -5.00}, Vector{-10.00, 0.00});
		bld2.line(9.0, Vector{0.00, -5.00}, Vector{10.00, 0.00});
		bld2.line(9.0, Vector{0.00, 0.00}, Vector{-10.00, 5.00});
		bld2.line(9.0, Vector{0.00, 0.00}, Vector{10.00, 5.00}); 
		bld.merge(bld2, -bld2.getCenter());
		std::cout << "Wings: " << bld2.getMass() << " kg" << std::endl;
		std::cout << "Subtotal: " << bld.getMass() << " kg" << std::endl;
		bld2.clear();

		r = 4.0;
		m = 3000 - bld.getMass();
		rho = m / (M_PI * r * r);
		bld2.disk(rho, Vector{0.00, 0.00}, r);
		bld.merge(bld2, -bld2.getCenter());
		std::cout << "Cabin: " << bld2.getMass() << " kg" << std::endl;
		std::cout << "Subtotal: " << bld.getMass() << " kg" << std::endl;
		bld2.clear();

		std::cout << "Total: " << bld.getMass() << " kg" << std::endl;
	// 3 000 kg total
		desc = bld.makeDesc();
		model = bld.makeModel();
	}
} model_builder;

const BodyDesc& ship_desc{model_builder.desc};//{ship_mass, 200.0 * ship_mass};
const Model& ship_model{model_builder.model};
const Float ship_mass = ship_desc.mass;
const Float ship_full_acceleration = 30.0;
const Float ship_engine_force = ship_mass * ship_engine_acceleration;
const Float ship_engine_acceleration = ship_full_acceleration / 2.0;

Ship::Ship(Vector _position) :
	Body(ship_desc)
{
	state.position = _position;
}

Ship::Ship(BodyState const& _state) :
	Body(ship_desc, _state)
{
}

void Ship::on_tick(Float duration)
{
	clampIt(engine_left, 1.0);
	clampIt(engine_right, 1.0);
	applyForceRel(Vector{0.0, ship_engine_force * engine_left}, Vector{-10.0, -7.0});
	applyForceRel(Vector{0.0, ship_engine_force * engine_right}, Vector{10.0, -7.0});
	Body::on_tick(duration);
}

ProgrammedShip::ProgrammedShip(Vector _position, Program const& _program) :
	Ship(_position),
	program(_program),
	current_statement(program.code.begin()),
	current_statement_shift(0.0)
{
}

ProgrammedShip::ProgrammedShip(BodyState const& _state, Program const& _program) :
	Ship(_state),
	program(_program),
	current_statement(program.code.begin()),
	current_statement_shift(0.0)
{
}

void ProgrammedShip::on_tick(Float duration)
{
	if(isRunning())
	{
		engine_left = current_statement->engine_left;
		engine_right = current_statement->engine_right;
		current_statement_shift += duration;
		if(current_statement_shift > current_statement->duration)
		{
			++current_statement;
			current_statement_shift = 0;
		}
	}
	Ship::on_tick(duration);
	if(!isRunning())
	{
		engine_left = 0.0;
		engine_right = 0.0;
	}
}

bool ProgrammedShip::isRunning()
{
	return current_statement != program.code.end();
}
