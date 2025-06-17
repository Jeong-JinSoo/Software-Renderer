#include "Rasterizer.h"

void Rasterizer::RasterizerTriangle(const RenderObject& obj, std::vector<uint32_t>& frameBuffer, std::vector<float>& depthBuffer, int screenWidth, int screenHeight /*const Texture& texture*/)
{
	// ȭ�鿡 �׸� �ﰢ�� ��� �����
	std::vector<Triangle> tris;
	tris.reserve(12);	// ť���� �ﰢ���� 12��

	for (int ti = 0; ti < 12; ++ti)
	{
		Triangle tri;
		for (int k = 0; k < 3; ++k)
		{
			int index = obj.ReturnIndex(ti * 3 + k);

			Point p = obj.finalVertex[index].vPoint;
			Point n = obj.finalVertex[index].vNormal;
			tri.v[k].x = p.x;
			tri.v[k].y = p.y;
			tri.v[k].z = p.z;
			tri.v[k].nx = n.x;
			tri.v[k].ny = n.y;
			tri.v[k].nz = n.z;
			tri.v[k].wInv = n.z;
			tri.v[k].u = obj.vertecies[index].u;
			tri.v[k].v = obj.vertecies[index].v;
		}
		tris.push_back(tri);
	}

	for (auto& tri : tris)
	{
		// ȭ�� �������� �ﰢ���� signed area (2��) ���
		float area2 =
			(tri.v[1].x - tri.v[0].x) * (tri.v[2].y - tri.v[0].y)
			- (tri.v[1].y - tri.v[0].y) * (tri.v[2].x - tri.v[0].x);

		// area2 > 0 �̸� CCW, < 0 �̸� CW
		// �ð� ���ΰ�? �ݽð� ���ΰ�? Ȯ���ϱ�
		bool isCW = (area2 < 0);

		// 1. �ٿ�� �ڽ� ���
		auto bb = Triangle::ComputeBoundingBox(tri, screenWidth, screenHeight);

		// 2. �ȼ� ������ ��ĵ (�ٿ�� �ڽ� ��)
		for (int y = bb.minY; y <= bb.maxY; ++y)
		{
			for (int x = bb.minX; x <= bb.maxX; x++)
			{
				float px = x + 0.5f;
				float py = y + 0.5f;

				// 3. ���� �Լ��� �ﰢ�� ���� Ȯ��
				float w0 = EdgeFunction(tri.v[1], tri.v[2], px, py);
				float w1 = EdgeFunction(tri.v[2], tri.v[0], px, py);
				float w2 = EdgeFunction(tri.v[0], tri.v[1], px, py);

				if (w0 > 0 || w1 > 0 || w2 > 0)continue;

				// 4. �ٸ����͸� ��ǥ & �۽���Ƽ�� ����
				float sum = w0 + w1 + w2;
				float alpha = w0 / sum;
				float beta = w1 / sum;
				float gamma = w2 / sum;

				// 1/w ������ ��
				float invW = alpha * tri.v[0].wInv
					+ beta * tri.v[1].wInv
					+ gamma * tri.v[2].wInv;
				float z = 1.0f / invW;

				int idx = y * screenWidth + x;

				// 5. ���� ���� �׽�Ʈ
				if (z < depthBuffer[idx])
				{
					depthBuffer[idx] = z;

					/// 6. �ؽ�ó ���ø� & ������ ���� ����
					//frameBuffer[idx] = texture.Sample(u, v);

					/// �ϴ��� �ؽ�ó�� �����Ƿ�
					//frameBuffer[idx] = 0xFFFFFFFFu; // ������ ���

					/// �븻�� �Կ��� ��;
					float nx = (alpha * tri.v[0].nx * tri.v[0].wInv
						+ beta * tri.v[1].nx * tri.v[1].wInv
						+ gamma * tri.v[2].nx * tri.v[2].wInv) * z;
					float ny = (alpha * tri.v[0].ny * tri.v[0].wInv
						+ beta * tri.v[1].ny * tri.v[1].wInv
						+ gamma * tri.v[2].ny * tri.v[2].wInv) * z;
					float nz = (alpha * tri.v[0].nz * tri.v[0].wInv
						+ beta * tri.v[1].nz * tri.v[1].wInv
						+ gamma * tri.v[2].nz * tri.v[2].wInv) * z;

					// ����ȭ
					float len = std::sqrt(nx * nx + ny * ny + nz * nz);
					nx /= len;  ny /= len;  nz /= len;

					/// 6. **�븻 ����� �÷���**
					// �븻 ([-1,1]) �� ([0,1]) �� ([0,255])
					uint8_t r = uint8_t((nx * 0.5f + 0.5f) * 255.0f);
					uint8_t g = uint8_t((ny * 0.5f + 0.5f) * 255.0f);
					uint8_t b = uint8_t((nz * 0.5f + 0.5f) * 255.0f);

					frameBuffer[idx] = (0xFFu << 24) | (r << 16) | (g << 8) | b;

					/// ����� ���� ���� L (���鿡�� ��� ��)
					//Point L = Point::Normalize(Point(0.0f, 0.0f, 1.0f));
					//// 1) ����Ʈ ��ǻ�� ���� ���
					//float diff = nx * L.x + ny * L.y + nz * L.z;
					//diff = diff < 0.0f ? 0.0f : diff;  // clamp
					//
					//// 2) [0,1] �� [0,255] ����
					//uint8_t c = static_cast<uint8_t>(diff * 255.0f);
					//
					//// 3) �����ӹ��ۿ� ȸ������ ���� (alpha = 0xFF)
					//frameBuffer[idx] = (0xFFu << 24) | (c << 16) | (c << 8) | c;
				}
			}
		}
	}
}

void Rasterizer::RasterizerLine(const RenderObject& obj, int screenWidth, int screenHeight, std::vector<uint32_t>& frameBuffer, std::vector<float>& depthBuffer)
{
	for (int i = 0; i < obj.indices.size(); i += 2)
	{
		Point v0 = obj.finalVertex[obj.indices[i]].vPoint;
		Point v1 = obj.finalVertex[obj.indices[i + 1]].vPoint;

		// ȭ�� ���� ��ǥ (�̹� ��ȯ �� ���� ������ �Ϸ��� ����)
		float x0 = v0.x, y0 = v0.y, z0 = v0.z, w0 = v0.w;
		float x1 = v1.x, y1 = v1.y, z1 = v1.z, w1 = v1.w;

		// Bresenham(����ȭ)�� �� ��� ��ǥ�� int�� ��ȯ
		int ix0 = static_cast<int>(std::round(x0));
		int iy0 = static_cast<int>(std::round(y0));
		int ix1 = static_cast<int>(std::round(x1));
		int iy1 = static_cast<int>(std::round(y1));

		int dx = abs(ix1 - ix0);
		int dy = abs(iy1 - iy0);
		int sx = (ix0 < ix1) ? 1 : -1;
		int sy = (iy0 < iy1) ? 1 : -1;
		int err = dx - dy;

		float len = std::sqrt((ix1 - ix0) * (ix1 - ix0) + (iy1 - iy0) * (iy1 - iy0));
		if (len < 1e-5f) len = 1.0f;

		// z, w �� ������ ���� t ����
		float total = std::max(dx, dy);

		int x = ix0;
		int y = iy0;
		uint32_t color = obj.finalVertex[obj.indices[i+1]].color;

		for (int j = 0; ; ++j)
		{
			float t = (total == 0) ? 0.0f : (float)j / total;
			// ���� (z/w, �� ��)
			float jz = z0 * (1 - t) + z1 * t;
			float jw = w0 * (1 - t) + w1 * t;

			// ���� ���� Ȯ��
			if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
			{
				int idx = y * screenWidth + x;

				// ���� �׽�Ʈ (w �Ǵ� z, �ʿ信 ���� ����)
				if (jz < depthBuffer[idx])
				{
					depthBuffer[idx] = jz;
					frameBuffer[idx] = color;
				}
			}

			if (x == ix1 && y == iy1)
				break;

			int e2 = 2 * err;
			if (e2 > -dy) { err -= dy; x += sx; }
			if (e2 < dx) { err += dx; y += sy; }
		}
	}
}

float Rasterizer::EdgeFunction(const Triangle::Vertex2D& A, const Triangle::Vertex2D& B, float Px, float Py)
{
    return (Px - A.x) * (B.y - A.y)
        - (Py - A.y) * (B.x - A.x);
}

uint32_t Texture::Sample(float u, float v) const
{
	return 0;
}
