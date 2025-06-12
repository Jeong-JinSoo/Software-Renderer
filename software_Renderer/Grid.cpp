#include "Grid.h"

Grid::Grid()
{
	type = RenderType::Line;

	int size = 4;

	vertecies.clear();
	indices.clear();

	int half = size / 2;
	int vertexIndex = 0;

	// X축 방향 선 (Z 고정)
	for (int z = -half; z <= half; ++z)
	{
		Point start(-half, 0, (float)z);
		Point end(half, 0, (float)z);

		vertecies.push_back(Vertex(start, Point())); // idx: vertexIndex
		vertecies.push_back(Vertex(end, Point()));   // idx: vertexIndex + 1

		indices.push_back(vertexIndex);
		indices.push_back(vertexIndex + 1);

		vertexIndex += 2;
	}

	// Z축 방향 선 (X 고정)
	for (int x = -half; x <= half; ++x)
	{
		Point start((float)x, 0, -half);
		Point end((float)x, 0, half);

		vertecies.push_back(Vertex(start, Point())); // idx: vertexIndex
		vertecies.push_back(Vertex(end, Point()));   // idx: vertexIndex + 1

		indices.push_back(vertexIndex);
		indices.push_back(vertexIndex + 1);

		vertexIndex += 2;
	}

	Initialize();
}

