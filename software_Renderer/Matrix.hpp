#pragma once
#include <cmath>

struct Matrix; // 전방선언

struct Point
{
	float x, y, z, w;

	Point() : x(0), y(0), z(0), w(1) {}
	Point(float px, float py, float pz, float pw = 1) : x(px), y(py), z(pz), w(pw) {}

	// Point + Point
	Point operator+(const Point& rhs) const
	{
		return Point(x + rhs.x, y + rhs.y, z + rhs.z, w); // w는 변환 유지
	}

	Point& operator+=(const Point& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;

		return *this;
	}

	// Point - Point
	Point operator-(const Point& rhs) const
	{
		return Point(x - rhs.x, y - rhs.y, z - rhs.z, w);
	}
	// Point * scalar
	Point operator*(float s) const {
		return Point(x * s, y * s, z * s, w);
	}

	// 행벡터 × 행렬 변환 : (x, y, z, w) × M
	Point operator*(const Matrix& m) const;

	// 단위벡터화(정규화)
	static Point Normalize(const Point& p) {
		float len = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
		if (len > 1e-8f) return Point(p.x / len, p.y / len, p.z / len, p.w);
		return p;
	}
	// 내적
	static float Dot(const Point& a, const Point& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
	// 외적
	static Point Cross(const Point& a, const Point& b) {
		return Point(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x,
			0
		);
	}
};

struct Matrix
{
	// 행벡터 표기 (row-major)
	union
	{
		float m[4][4];	// 2차원 배열
		struct			// 쌩 함수 버전
		{
			float a1, a2, a3, a4;
			float b1, b2, b3, b4;
			float c1, c2, c3, c4;
			float d1, d2, d3, d4;
		};
	};


	Matrix() { // 단위행렬
		for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) m[i][j] = (i == j) ? 1.0f : 0.0f;
	}

	Matrix(float a1, float a2, float a3, float a4,
		float b1, float b2, float b3, float b4,
		float c1, float c2, float c3, float c4,
		float d1, float d2, float d3, float d4)
	{
		m[0][0] = a1; m[0][1] = a2; m[0][2] = a3; m[0][3] = a4;
		m[1][0] = b1; m[1][1] = b2; m[1][2] = b3; m[1][3] = b4;
		m[2][0] = c1; m[2][1] = c2; m[2][2] = c3; m[2][3] = c4;
		m[3][0] = d1; m[3][1] = d2; m[3][2] = d3; m[3][3] = d4;
	}

	void SetIdentity()
	{
		for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) m[i][j] = (i == j) ? 1.0f : 0.0f;
	}

	// 행벡터 × 행렬 연산 (row * matrix)
	Matrix operator*(const Matrix& rhs) const
	{
		Matrix res;
		for (int r = 0; r < 4; ++r)
			for (int c = 0; c < 4; ++c) {
				res.m[r][c] = 0;
				for (int k = 0; k < 4; ++k)
					res.m[r][c] += m[r][k] * rhs.m[k][c];
			}
		return res;
	}

	// 행렬 + 행렬
	Matrix operator+(const Matrix& rhs) const
	{
		Matrix res;
		for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) res.m[i][j] = m[i][j] + rhs.m[i][j];
		return res;
	}

	// 행렬 - 행렬
	Matrix operator-(const Matrix& rhs) const
	{
		Matrix res;
		for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) res.m[i][j] = m[i][j] - rhs.m[i][j];
		return res;
	}

	// 전치행렬
	Matrix Transposed() const
	{
		Matrix t;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				t.m[i][j] = m[j][i];
		return t;
	}

	// 행벡터기준 역행렬(생략, 필요하면 구현)
	Matrix Inverse()
	{
		return Matrix();
	};

	// 행렬식(Determinant) (생략, 필요하면 구현)
	float GetDeterminant()
	{
	
	};

	// (보통은 변환행렬에만 사용)  
	// ... (기타 필요시 추가)
};

// 행벡터 × 행렬 연산 (Point * Matrix)
inline Point Point::operator*(const Matrix& mat) const {
	Point res;
	res.x = x * mat.m[0][0] + y * mat.m[1][0] + z * mat.m[2][0] + w * mat.m[3][0];
	res.y = x * mat.m[0][1] + y * mat.m[1][1] + z * mat.m[2][1] + w * mat.m[3][1];
	res.z = x * mat.m[0][2] + y * mat.m[1][2] + z * mat.m[2][2] + w * mat.m[3][2];
	res.w = x * mat.m[0][3] + y * mat.m[1][3] + z * mat.m[2][3] + w * mat.m[3][3];
	return res;
}

// 행벡터용 Quaternion
struct Quaternion {
	float x, y, z, w;
	Quaternion() : x(0), y(0), z(0), w(1) {}
	Quaternion(float _x, float _y, float _z, float _w)
		: x(_x), y(_y), z(_z), w(_w) {}

	static Quaternion FromAxisAngle(const Point& axis, float angleRad) {
		float half = angleRad * 0.5f;
		float s = sinf(half);
		return Quaternion(
			axis.x * s,
			axis.y * s,
			axis.z * s,
			cosf(half)
		);
	}

	static Quaternion FromEuler(float xRad, float yRad, float zRad) {
		float cx = cosf(xRad * 0.5f);
		float sx = sinf(xRad * 0.5f);
		float cy = cosf(yRad * 0.5f);
		float sy = sinf(yRad * 0.5f);
		float cz = cosf(zRad * 0.5f);
		float sz = sinf(zRad * 0.5f);
		// ZYX (Roll-Pitch-Yaw, 행벡터 기준)
		return Quaternion(
			sx * cy * cz + cx * sy * sz, // x
			cx * sy * cz - sx * cy * sz, // y
			cx * cy * sz + sx * sy * cz, // z
			cx * cy * cz - sx * sy * sz  // w
		);
	}

	Quaternion operator*(const Quaternion& q) const {
		return Quaternion(
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w,
			w * q.w - x * q.x - y * q.y - z * q.z
		);
	}

	void Normalize() {
		float len = std::sqrt(x * x + y * y + z * z + w * w);
		if (len > 0.0f) { x /= len; y /= len; z /= len; w /= len; }
	}

	// 행벡터용 4x4 회전행렬 변환
	Matrix ToMatrix() const {
		float xx = x * x, yy = y * y, zz = z * z;
		float xy = x * y, xz = x * z, yz = y * z;
		float wx = w * x, wy = w * y, wz = w * z;
		// 행벡터 기준:
		return Matrix(
			1 - 2 * yy - 2 * zz, 2 * xy + 2 * wz, 2 * xz - 2 * wy, 0,
			2 * xy - 2 * wz, 1 - 2 * xx - 2 * zz, 2 * yz + 2 * wx, 0,
			2 * xz + 2 * wy, 2 * yz - 2 * wx, 1 - 2 * xx - 2 * yy, 0,
			0, 0, 0, 1
		);
	}
};
