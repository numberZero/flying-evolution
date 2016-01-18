#include "model.hxx"
#include <stdexcept>
#include <GL/gl.h>

void Model::draw() const
{
	glBegin(GL_POINTS);
	drawIndices(indices1);
	glEnd();
	glBegin(GL_LINES);
	drawIndices(indices2);
	glEnd();
	glBegin(GL_TRIANGLES);
	drawIndices(indices3);
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

void Model::drawIndices(Indices const& indices) const
{
	for(Index index: indices)
	{
		Vertex const& vertex(vertices[index]);
		glVertex2d(vertex.pos[0], vertex.pos[1]);
	}
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
	m.vertices.resize(points.size() + lines.size() + triangles.size());
	m.indices1.resize(points.size());
	m.indices2.resize(lines.size());
	m.indices3.resize(triangles.size());
	Index i = 0;
	for(Index j = 0; j != points.size(); ++i, ++j)
	{
		m.indices1[j] = i;
		m.vertices[i].pos = shift + points[j];
	}
	for(Index j = 0; j != lines.size(); ++i, ++j)
	{
		m.indices2[j] = i;
		m.vertices[i].pos = shift + lines[j];
	}
	for(Index j = 0; j != triangles.size(); ++i, ++j)
	{
		m.indices3[j] = i;
		m.vertices[i].pos = shift + triangles[j];
	}
	return std::move(m);
}

void ModelBuilder::merge(ModelBuilder const& b, Vector position)
{
	mass += b.mass;
	mass_shift += b.mass * position + b.mass_shift;
	inertia_tensor += b.mass * position.squaredNorm() + 2 * position.dot(b.mass_shift) + b.inertia_tensor;

	mergeList(points, b.points, position);
	mergeList(lines, b.lines, position);
	mergeList(triangles, b.triangles, position);
}

void ModelBuilder::mergeList(Vertices& to, Vertices const& from, Vector position)
{
	for(Vector const& v: from)
		to.emplace_back(position + v);
}

void ModelBuilder::centralize()
{
	Vector shift = -getCenter();
	inertia_tensor = mass * shift.squaredNorm() + 2 * shift.dot(mass_shift) + inertia_tensor;
	mass_shift.setZero();
	shiftList(points, shift);
	shiftList(lines, shift);
	shiftList(triangles, shift);
}

void ModelBuilder::shiftList(Vertices& vs, Vector by)
{
	for(auto& v: vs)
		v += by;
}

void ModelBuilder::clear()
{
	mass = 0;
	mass_shift.setZero();
	inertia_tensor = 0;
	points.clear();
	lines.clear();
	triangles.clear();
}

void ModelBuilder::point(Float point_density, Vector position)
{
	points.emplace_back(position);

	mass += point_density;
	mass_shift += point_density * position;
	inertia_tensor += point_density * position.squaredNorm();
}

void ModelBuilder::circle(Float linear_density, Vector center, Float radius)
{
	NOT_IMPLEMENTED;
}

void ModelBuilder::line(Float linear_density, Vector a, Vector b)
{
	lines.emplace_back(a);
	lines.emplace_back(b);

	Vector d = b - a;
	Float len = d.norm();
	Float line_mass = linear_density * len;

	mass += line_mass;
	mass_shift += line_mass * ((a + b) / 2);
	inertia_tensor += line_mass * (
		(std::isnormal(d[0]) ? (pown(b[0], 3) - pown(a[0], 3)) / (3 * d[0]) : 0) +
		(std::isnormal(d[1]) ? (pown(b[1], 3) - pown(a[1], 3)) / (3 * d[1]) : 0));
}

void ModelBuilder::disk(Float density, Vector center, Float radius)
{
	NOT_IMPLEMENTED;
}

void ModelBuilder::triangle(Float linear_density, Vector a, Vector b, Vector c)
{
	NOT_IMPLEMENTED;
}

void ModelBuilder::quad(Float density, Vector vertex, Vector side1, Vector side2)
{
	NOT_IMPLEMENTED;
}
