#pragma once
#include <list>
#include <stdexcept>
#include <Eigen/Core>

typedef double Float;
typedef Eigen::Matrix<Float, 2, 1> Vector;

struct BodyDesc
{
	Float mass;
	Float rotation_resistance;
};

enum class StepState
{
	Opaque,
	Ready,
	Dirty,
};

struct Body
{
// static data
	BodyDesc const& desc;

// body state
	Vector position;
	Vector velocity;
	Float rotation;
	Float angular_veloctiy;

// temporary storage
	StepState stst = StepState::Opaque;
	Vector force;
	Float momentum;

// methods
	explicit Body(BodyDesc const& description);
	virtual ~Body() = default;
	virtual void on_before_tick();
	void on_after_tick();
	void on_tick(Float duration);
	void applyForceRel(Vector value, Vector shift);
};

struct Simulation
{
	Float global_time = 0;
	std::list<Body*> bodies;
	StepState stst = StepState::Opaque;
	
	void on_before_tick();
	void on_after_tick();
	void on_tick(Float duration);
};

struct invalid_state_error: public std::logic_error
{
	static std::string format_message(const std::string func, StepState expected, StepState real);
	invalid_state_error(const std::string func, StepState expected, StepState real);
};

inline std::string to_string(StepState stst)
{
	switch(stst)
	{
		case StepState::Opaque:
			return "Opaque";
		case StepState::Ready:
			return "Ready";
		case StepState::Dirty:
			return "Dirty";
	};
}
