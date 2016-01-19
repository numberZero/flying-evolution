#include "model.hxx"
#include <stdexcept>
#include <GL/gl.h>

void Model::draw() const
{
	glBegin(GL_TRIANGLES);
	for(Index index: indices)
	{
		Vertex const& vertex(vertices[index]);
		glVertex2d(vertex.pos[0], vertex.pos[1]);
	}
	glEnd();
}

void Model::draw(Vector position, Float rotation, Float scale) const
{
	glPushMatrix();
	glTranslated(position[0], position[1], 0.0);
	glRotated((180.0 / M_PI) * rotation, 0.0, 0.0, 1.0);
	glScaled(scale, scale, scale);
	draw();
	glPopMatrix();
}

ModelBuilder::ModelBuilder()
{
	clear();
}

Float ModelBuilder::getMass() const
{
	return mass;
}

Vector ModelBuilder::getCenter() const
{
	return -mass_shift / mass;
}

BodyDesc ModelBuilder::makeDesc() const
{
	return{mass, inertia_tensor};
}

Model ModelBuilder::makeModel() const
{
	Model m;
	Vector shift = -getCenter();
	m.vertices.resize(vertices.size());
	m.indices.resize(indices.size());
	for(Index i = 0; i != indices.size(); ++i)
		m.indices[i] = indices[i];
	for(Index j = 0; j != vertices.size(); ++j)
		m.vertices[j].pos = shift + vertices[j];
	return std::move(m);
}

void ModelBuilder::merge(ModelBuilder const& b, Vector position)
{
	mass += b.mass;
	mass_shift += b.mass * position + b.mass_shift;
	inertia_tensor += b.mass * position.squaredNorm() + 2 * position.dot(b.mass_shift) + b.inertia_tensor;

	Index bv = vertices.size();
	Index bi = indices.size();
	vertices.resize(bv + b.vertices.size());
	indices.resize(bi + b.indices.size());
	auto pv = vertices.begin() + bv;
	auto pi = indices.begin() + bi;
	for(Vector const& v: b.vertices)
		*(pv++) = position + v;
	for(Index const& i: b.indices)
		*(pi++) = bv + i;
}

void ModelBuilder::centralize()
{
	Vector shift = -getCenter();
	inertia_tensor = mass * shift.squaredNorm() + 2 * shift.dot(mass_shift) + inertia_tensor;
	mass_shift.setZero();
	for(auto& v: vertices)
		v += shift;
}

void ModelBuilder::clear()
{
	mass = 0;
	mass_shift.setZero();
	inertia_tensor = 0;
	vertices.clear();
	indices.clear();
}

void ModelBuilder::point(Float point_density, Vector position, Float visual_radius, int visual_segments)
{
	Index base = vertices.size();
	vertices.emplace_back(position);
	for(int k = 0; k != visual_segments; ++k)
	{
		int index1 = base + 1 + k;
		int index2 = base + 1 + ((k + 1) % visual_segments);
		Float phi = 2.0 * M_PI * k / visual_segments;
		Float x = std::sin(phi);
		Float y = std::cos(phi);
		vertices.emplace_back(position + visual_radius * Vector(x, y));
		indices.emplace_back(base);
		indices.emplace_back(index1);
		indices.emplace_back(index2);
	}

	mass += point_density;
	mass_shift += point_density * position;
	inertia_tensor += point_density * position.squaredNorm();
}

void ModelBuilder::circle(Float linear_density, Vector center, Float radius, Float visual_thickness, int visual_segments)
{
	Index base = vertices.size();
	Float inner_radius = radius - 0.5 * visual_thickness;
	Float outer_radius = radius + 0.5 * visual_thickness;
	for(int k = 0; k != visual_segments; ++k)
	{
		int index1 = base + 2 * k;
		int index2 = base + 2 * ((k + 1) % visual_segments);
		Float phi = 2.0 * M_PI * k / visual_segments;
		Float x = std::sin(phi);
		Float y = std::cos(phi);
		vertices.emplace_back(center + inner_radius * Vector(x, y));
		vertices.emplace_back(center + outer_radius * Vector(x, y));
		indices.emplace_back(index1);
		indices.emplace_back(index1 + 1);
		indices.emplace_back(index2 + 1);
		indices.emplace_back(index1);
		indices.emplace_back(index2);
		indices.emplace_back(index2 + 1);
	}

	Float new_mass = 2 * M_PI * radius * linear_density;
	mass += new_mass;
	mass_shift += new_mass * center;
	inertia_tensor += new_mass * (center.squaredNorm() + pown(radius, 2));
}

void ModelBuilder::line(Float linear_density, Vector a, Vector b, Float visual_thickness)
{
	Vector d = b - a;
	Float len = d.norm();
	Float line_mass = linear_density * len;

	Index base = vertices.size();
	Vector shift(d[1], -d[0]);
	shift *= 0.5 * visual_thickness / len;
	vertices.emplace_back(a + shift);
	vertices.emplace_back(a - shift);
	vertices.emplace_back(b + shift);
	vertices.emplace_back(b - shift);
	indices.emplace_back(base);
	indices.emplace_back(base + 1);
	indices.emplace_back(base + 2);
	indices.emplace_back(base + 3);
	indices.emplace_back(base + 1);
	indices.emplace_back(base + 2);

	mass += line_mass;
	mass_shift += line_mass * ((a + b) / 2);
	inertia_tensor += line_mass * (
		(std::isnormal(d[0]) ? (pown(b[0], 3) - pown(a[0], 3)) / (3 * d[0]) : 0) +
		(std::isnormal(d[1]) ? (pown(b[1], 3) - pown(a[1], 3)) / (3 * d[1]) : 0));
}

void ModelBuilder::disk(Float density, Vector center, Float radius, int visual_segments)
{
	Index base = vertices.size();
	vertices.emplace_back(center);
	for(int k = 0; k != visual_segments; ++k)
	{
		int index1 = base + 1 + k;
		int index2 = base + 1 + ((k + 1) % visual_segments);
		Float phi = 2.0 * M_PI * k / visual_segments;
		Float x = std::sin(phi);
		Float y = std::cos(phi);
		vertices.emplace_back(center + radius * Vector(x, y));
		indices.emplace_back(base);
		indices.emplace_back(index1);
		indices.emplace_back(index2);
	}

	Float new_mass = M_PI * pown(radius, 2) * density;
	mass += new_mass;
	mass_shift += new_mass * center;
	inertia_tensor += new_mass * (center.squaredNorm() + 0.5 * pown(radius, 2));
}

void ModelBuilder::triangle(Float density, Vector a, Vector b, Vector c)
{
	NOT_IMPLEMENTED;
}

void ModelBuilder::quad(Float density, Vector vertex, Vector side1, Vector side2)
{
	NOT_IMPLEMENTED;
}
