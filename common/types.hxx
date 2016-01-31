#pragma once
#include <stdexcept>
#include <string>
#include <Eigen/Core>

typedef double Float;
typedef Eigen::Matrix<Float, 2, 1> Vector;

struct BodyDesc
{
	Float mass;
	Float rotation_resistance;
};

struct not_implemented: public std::logic_error
{
	not_implemented(std::string funcname) :
		logic_error("Function not implemented: " + funcname)
	{
	}
};

#define NOT_IMPLEMENTED	throw not_implemented(__PRETTY_FUNCTION__)

inline Float pown(Float x, unsigned n)
{
	Float value = 1.0;
	while(n)
	{
		if(n & 1)
			value *= x;
		n >>= 1;
		x *= x;
	}
	return value;
}

inline void clampIt(Float& variable, Float max_abs)
{
	assert(max_abs >= 0);
	if(variable > max_abs)
		variable = max_abs;
	else if(variable < -max_abs)
		variable = -max_abs;
}

inline Float clamp(Float value, Float max_abs)
{
	clampIt(value, max_abs);
	return value;
}
