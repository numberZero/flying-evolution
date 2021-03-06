#include "simulation.hxx"
#include <cmath>

std::string invalid_state_error::format_message(const std::string func, StepState expected, StepState real)
{
	return "Function " + func + " is called when object is not in the needed state: " + std::to_string(expected) + " instead of " + std::to_string(real);
}

invalid_state_error::invalid_state_error(const std::string func, StepState expected, StepState real):
	logic_error(format_message(func, expected, real))
{
}

BodyState::BodyState() :
	position(0, 0), velocity(0, 0), rotation(0), angular_veloctiy(0)
{
}

#define check_state(expected)	\
	if(stst != expected)	\
		throw invalid_state_error(__PRETTY_FUNCTION__, expected, stst)

Body::Body(BodyDesc const& description) :
	desc(description), state()
{
}

Body::Body(BodyDesc const& description, BodyState const& initial_state) :
	desc(description), state(initial_state)
{

}

void Body::on_before_tick(Float global_time)
{
	check_state(StepState::Opaque);
	force = Vector(0, 0);
	momentum = 0;
	stst = StepState::Ready;
}

void Body::on_after_tick()
{
	check_state(StepState::Dirty);
	stst = StepState::Opaque;
}

void Body::on_tick(Float duration)
{
	check_state(StepState::Ready);
	state.velocity += duration / desc.mass * force;
	state.angular_veloctiy += duration / desc.rotation_resistance * momentum;
	state.position += duration * state.velocity;
	state.rotation += duration * state.angular_veloctiy;
	stst = StepState::Dirty;
}

void Body::applyForceRel(Vector value, Vector shift)
{
	Float ang_force(value[1] * shift[0] - value[0] * shift[1]);
	Float c = std::cos(state.rotation);
	Float s = std::sin(state.rotation);
	Vector lin_force(
		c * value[0] - s * value[1],
		s * value[0] + c * value[1]);
	momentum += ang_force;
	force += lin_force;
}

void Simulation::on_before_tick()
{
	check_state(StepState::Opaque);
	for(Body* body: bodies)
		body->on_before_tick(global_time);
	stst = StepState::Ready;
}

void Simulation::on_after_tick()
{
	check_state(StepState::Dirty);
	for(Body* body: bodies)
		body->on_after_tick();
	stst = StepState::Opaque;
}

void Simulation::on_tick(Float duration)
{
	check_state(StepState::Ready);
	for(Body* body: bodies)
		body->on_tick(duration);
	global_time += duration;
	stst = StepState::Dirty;
}
