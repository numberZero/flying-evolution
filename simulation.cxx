#include "simulation.hxx"

std::string invalid_state_error::format_message(const std::string func, StepState expected, StepState real)
{
	return "Function " + func + " is called when object is not in the needed state: " + to_string(expected) + " instead of " + to_string(real);
}

invalid_state_error::invalid_state_error(const std::string func, StepState expected, StepState real):
	logic_error(format_message(func, expected, real))
{
}

#define check_state(expected)	\
	if(stst != expected)	\
		throw invalid_state_error(__PRETTY_FUNCTION__, expected, stst)

Body::Body(BodyDesc const& description) :
        desc(description), position(0, 0), velocity(0, 0), rotation(0), angular_veloctiy(0)
{
}

void Body::on_before_tick()
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
        velocity += duration / desc.mass * force;
        angular_veloctiy += duration / desc.rotation_resistance * momentum;
        position += duration * velocity;
        rotation += duration * angular_veloctiy;
	stst = StepState::Dirty;
}

void Body::applyForceRel(Vector value, Vector shift)
{
        Float ang_force(value[1] * shift[0] - value[0] * shift[1]);
        Float c = cos(rotation);
        Float s = sin(rotation);
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
		body->on_before_tick();
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
