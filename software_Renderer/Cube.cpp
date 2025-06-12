#include "Cube.h"
#include "Camera.h"

Cube::Cube()
{
	type = RenderType::Face;

	// Á¤Á¡
	vertecies.resize(8);
	vertecies[0] = Vertex(Point( 1,  1, -1),	Point());
	vertecies[1] = Vertex(Point(-1,  1, -1),	Point());
	vertecies[2] = Vertex(Point( 1, -1, -1),	Point());
	vertecies[3] = Vertex(Point(-1, -1, -1),	Point());
	vertecies[4] = Vertex(Point( 1,  1,  1),	Point());
	vertecies[5] = Vertex(Point(-1,  1,  1),	Point());
	vertecies[6] = Vertex(Point( 1, -1,  1),	Point());
	vertecies[7] = Vertex(Point(-1, -1,  1),	Point());


	// ÀÎµ¦½º
	indices.resize(36);
	indices[0] = 4, indices[1] = 0,  indices[2] = 2;
	indices[3] = 4, indices[4] = 2,  indices[5] = 6;
	indices[6] = 5, indices[7] = 1,  indices[8] = 0;
	indices[9] = 5, indices[10] = 0, indices[11] = 4;
	indices[12] = 7, indices[13] = 3, indices[14] = 1;
	indices[15] = 7, indices[16] = 1, indices[17] = 5;
	indices[18] = 6, indices[19] = 2, indices[20] = 3;
	indices[21] = 6, indices[22] = 3, indices[23] = 7;
	indices[24] = 5, indices[25] = 4, indices[26] = 6;
	indices[27] = 5, indices[28] = 6, indices[29] = 7;
	indices[30] = 0, indices[31] = 1, indices[32] = 3;
	indices[33] = 0, indices[34] = 3, indices[35] = 2;

	this->Initialize();
}