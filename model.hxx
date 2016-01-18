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
	Indices indices1; // points
	Indices indices2; // lines
	Indices indices3; // triangles

	void draw() const;
	void draw(Vector position, Float rotation, Float scale = 1.0) const;

	void drawIndices(Indices const& indices) const;
};

class ModelBuilder
{
private:
	typedef std::deque<Vector> Vertices;

	Vector mass_shift;
	Float mass;
	Float inertia_tensor;

	Vertices points;
	Vertices lines;
	Vertices triangles;

public:
	ModelBuilder();

	Float getMass() const;
	Vector getCenter() const;

	BodyDesc makeDesc() const;
	Model makeModel() const; // (0; 0) in the model is always the center of mass

	void merge(ModelBuilder const& b, Vector position);
	void centralize();
	void clear();

	void point(Float mass, Vector position);

	void circle(Float linear_density, Vector center, Float radius);
	void line(Float linear_density, Vector a, Vector b);

	void disk(Float density, Vector center, Float radius);
	void triangle(Float linear_density, Vector a, Vector b, Vector c);
	void quad(Float density, Vector vertex, Vector side1, Vector side2);

private:
	static void mergeList(Vertices& to, Vertices const& from, Vector position);
	static void shiftList(Vertices& vs, Vector by);
};
