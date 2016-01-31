#pragma once
#include <deque>
#include <set>
#include <vector>
#include "types.hxx"

struct Vertex
{
	Vector pos;

// 	Vertex() = default;
// 	Vertex(Vector _pos) :
// 		pos(_pos)
// 	{
// 	}
};

typedef unsigned long Index;

struct Model
{
	typedef std::vector<Vertex> Vertices;
	typedef std::vector<Index> Indices;

	Vertices vertices;
	Indices indices;

	void draw() const;
	void draw(Vector position, Float rotation, Float scale = 1.0) const;
};

class ModelBuilder
{
private:
	typedef std::deque<Vector> Vertices;
	typedef std::deque<Index> Indices;

	Vector mass_shift;
	Float mass;
	Float inertia_tensor;

	Vertices vertices;
	Indices indices;

public:
	ModelBuilder();

	Float getMass() const;
	Vector getCenter() const;

	BodyDesc makeDesc() const;
	Model makeModel() const; // (0; 0) in the model is always the center of mass

	void merge(ModelBuilder const& b, Vector position);
	void centralize();
	void clear();

	void point(Float mass, Vector position, Float visual_radius = 1.0, int visual_segments = 6);

	void circle(Float linear_density, Vector center, Float radius, Float visual_thickness = 1.0, int visual_segments = 24);
	void line(Float linear_density, Vector a, Vector b, Float visual_thickness = 1.0);

	void disk(Float density, Vector center, Float radius, int visual_segments = 24);
	void triangle(Float density, Vector a, Vector b, Vector c);
	void quad(Float density, Vector vertex, Vector side1, Vector side2);
};
