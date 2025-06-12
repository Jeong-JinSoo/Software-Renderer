#include "Axis_Widget.h"

Axis_Widget::Axis_Widget()
{
	type = RenderType::Line;

	vertecies.resize(4);
	vertecies[0] = Vertex(Point(0, 0, 0), Point()); // 중점
	vertecies[1] = Vertex(Point(1, 0, 0), Point()); // x축
	vertecies[2] = Vertex(Point(0, 1, 0), Point()); // y축
	vertecies[3] = Vertex(Point(0, 0, 1), Point()); // z축

	vertecies[1].color = 0xFFFF0000;
	vertecies[2].color = 0xFF00FF00;
	vertecies[3].color = 0xFF0000FF;

	indices.resize(6);
	indices[0] = 0, indices[1] = 1;	// x축
	indices[2] = 0, indices[3] = 2;	// y축
	indices[4] = 0, indices[5] = 3;	// z축

	Initialize();
}

Axis_Widget::~Axis_Widget()
{

}
