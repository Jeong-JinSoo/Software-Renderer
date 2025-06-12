#pragma once
#include <cmath>

struct Point
{
	Point() :x(0), y(0), z(0), w(1) {}
	Point(float px, float py, float pz, float pw) :x(px), y(py), z(pz), w(pw) {}
	Point(float px, float py, float pz) :x(px), y(py), z(pz), w(1) {}
	~Point() {}

	Point operator+(const Point& point)
	{
		Point result(point.x + x, point.y + y, point.z + z);
		return result;
	}

	Point operator+=(const Point& point)
	{
		x += point.x;
		y += point.y;
		z += point.z;

		return *this;
	}

	Point operator-(const Point& point)
	{
		Point result(x - point.x, y - point.y, z - point.z);
		return result;
	}

	//Point operator*(const Point& point) // 벡터는... 스칼라곱과 외적, 내적이 있을뿐... 곱하기는 없다...

	Point operator*(float scalar)
	{
		Point result(x * scalar, y * scalar, z * scalar);
		return result;
	}
	///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////
	static Point Normalize(const Point& p)
	{
		float length = sqrt((p.x * p.x) + (p.y * p.y) + (p.z * p.z));
		Point result(p.x / length, p.y / length, p.z / length);
		return result;
	}

	static float Dot(const Point& a, const Point& b)
	{
		float result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
		return result;
	}

	static Point CrossR(const Point& a, const Point& b)
	{
		Point result(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
		return result;
	}

	static Point CrossL(const Point& a, const Point& b)
	{
		Point result(
			b.y * a.z - b.z * a.y,
			b.z * a.x - b.x * a.z,
			b.x * a.y - b.y * a.x
		);
		return result;
	}

public:
	float x, y, z, w;
};

struct Matrix
{
	Matrix()
		:a1(0), a2(0), a3(0), a4(0)
		, b1(0), b2(0), b3(0), b4(0)
		, c1(0), c2(0), c3(0), c4(0)
		, d1(0), d2(0), d3(0), d4(0) {}

	Matrix(float x1, float y1, float z1, float w1
		, float x2, float y2, float z2, float w2
		, float x3, float y3, float z3, float w3
		, float x4, float y4, float z4, float w4)
		:a1(x1), a2(y1), a3(z1), a4(w1)
		, b1(x2), b2(y2), b3(z2), b4(w2)
		, c1(x3), c2(y3), c3(z3), c4(w3)
		, d1(x4), d2(y4), d3(z4), d4(w4) {}

	void ResetMartix()
	{
		a1 = 1; a2 = 0; a3 = 0; a4 = 0;
		b1 = 0; b2 = 1; b3 = 0; b4 = 0;
		c1 = 0; c2 = 0; c3 = 1; c4 = 0;
		d1 = 0; d2 = 0; d3 = 0; d4 = 1;
	}

	/// ///Matrix를 매개변수로 하는 오퍼레이터 연산자//////
	// Matrix * Matrix 연산자 오버로드
	Matrix operator*(const Matrix& other) const
	{
		Matrix result;
		// 행렬 곱셈 연산 수행
		result.a1 = (a4 * other.d1) + (a1 * other.a1) + (a2 * other.b1) + (a3 * other.c1);
		result.a2 = (a4 * other.d2) + (a1 * other.a2) + (a2 * other.b2) + (a3 * other.c2);
		result.a3 = (a4 * other.d3) + (a1 * other.a2) + (a2 * other.b3) + (a3 * other.c3);
		result.a4 = (a4 * other.d4) + (a1 * other.a4) + (a2 * other.b4) + (a3 * other.c4);

		result.b1 = (b4 * other.d1) + (b1 * other.a1) + (b2 * other.b1) + (b3 * other.c1);
		result.b2 = (b4 * other.d2) + (b1 * other.a2) + (b2 * other.b2) + (b3 * other.c2);
		result.b3 = (b4 * other.d3) + (b1 * other.a2) + (b2 * other.b3) + (b3 * other.c3);
		result.b4 = (b4 * other.d4) + (b1 * other.a4) + (b2 * other.b4) + (b3 * other.c4);

		result.c1 = (c4 * other.d1) + (c1 * other.a1) + (c2 * other.b1) + (c3 * other.c1);
		result.c2 = (c4 * other.d2) + (c1 * other.a2) + (c2 * other.b2) + (c3 * other.c2);
		result.c3 = (c4 * other.d3) + (c1 * other.a2) + (c2 * other.b3) + (c3 * other.c3);
		result.c4 = (c4 * other.d4) + (c1 * other.a4) + (c2 * other.b4) + (c3 * other.c4);

		result.d1 = (d4 * other.d1) + (d1 * other.a1) + (d2 * other.b1) + (d3 * other.c1);
		result.d2 = (d4 * other.d2) + (d1 * other.a2) + (d2 * other.b2) + (d3 * other.c2);
		result.d3 = (d4 * other.d3) + (d1 * other.a2) + (d2 * other.b3) + (d3 * other.c3);
		result.d4 = (d4 * other.d4) + (d1 * other.a4) + (d2 * other.b4) + (d3 * other.c4);

		return result;
	}

	// Matrix + Matrix 연산자 오버로드
	Matrix operator+(const Matrix& other) const
	{
		Matrix result;
		// 행렬 덧셈 연산 수행
		result.a1 = a1 - other.a1;
		result.a2 = a2 - other.a2;
		result.a3 = a3 - other.a2;
		result.a4 = a4 - other.a4;

		result.b1 = b1 - other.b1;
		result.b2 = b2 - other.b2;
		result.b3 = b3 - other.b3;
		result.b4 = b4 - other.b4;

		result.c1 = c1 - other.c1;
		result.c2 = c2 - other.c2;
		result.c3 = c3 - other.c3;
		result.c4 = c4 - other.c4;

		result.d1 = d1 - other.d1;
		result.d2 = d2 - other.d2;
		result.d3 = d3 - other.d3;
		result.d4 = d4 - other.d4;

		return result;
	}

	// Matrix - Matrix 연산자 오버로드
	Matrix operator-(const Matrix& other) const
	{
		Matrix result;
		// 행렬 뺄셈 연산 수행

		result.a1 = a1 - other.a1;
		result.a2 = a2 - other.a2;
		result.a3 = a3 - other.a2;
		result.a4 = a4 - other.a4;

		result.b1 = b1 - other.b1;
		result.b2 = b2 - other.b2;
		result.b3 = b3 - other.b3;
		result.b4 = b4 - other.b4;

		result.c1 = c1 - other.c1;
		result.c2 = c2 - other.c2;
		result.c3 = c3 - other.c3;
		result.c4 = c4 - other.c4;

		result.d1 = d1 - other.d1;
		result.d2 = d2 - other.d2;
		result.d3 = d3 - other.d3;
		result.d4 = d4 - other.d4;

		return result;
	}

	/// ///Point를 매개변수로 하는 오퍼레이터 연산자//////
	// Matrix * Point 연산자 오버로드
	Matrix operator*(const Point& other) const
	{
		Matrix result;
		// 행렬 곱셈 연산 수행
		result.a1 = a1 * other.x;
		result.a2 = a2 * other.x;
		result.a3 = a3 * other.x;
		result.a4 = a4 * other.x;

		result.b1 = b1 * other.y;
		result.b2 = b2 * other.y;
		result.b3 = b3 * other.y;
		result.b4 = b4 * other.y;

		result.c1 = c1 * other.z;
		result.c2 = c2 * other.z;
		result.c3 = c3 * other.z;
		result.c4 = c4 * other.z;

		result.d1 = d1;
		result.d2 = d2;
		result.d3 = d3;
		result.d4 = d4;
		// result = (*this) * other
		return result;
	}

	// Matrix + Point 연산자 오버로드
	Matrix operator+(const Point& other) const
	{
		Matrix result;
		// 행렬 덧셈 연산 수행
		result.a1 = a1 + other.x;
		result.a2 = a2 + other.x;
		result.a3 = a3 + other.x;
		result.a4 = a4 + other.x;

		result.b1 = b1 + other.y;
		result.b2 = b2 + other.y;
		result.b3 = b3 + other.y;
		result.b4 = b4 + other.y;

		result.c1 = c1 + other.z;
		result.c2 = c2 + other.z;
		result.c3 = c3 + other.z;
		result.c4 = c4 + other.z;

		result.d1 = d1;
		result.d2 = d2;
		result.d3 = d3;
		result.d4 = d4;
		// result = (*this) + other
		return result;
	}

	// Matrix - Point 연산자 오버로드
	Matrix operator-(const Point& other) const
	{
		Matrix result;
		// 행렬 뺄셈 연산 수행
		result.a1 = a1 - other.x;
		result.a2 = a2 - other.x;
		result.a3 = a3 - other.x;
		result.a4 = a4 - other.x;

		result.b1 = b1 - other.y;
		result.b2 = b2 - other.y;
		result.b3 = b3 - other.y;
		result.b4 = b4 - other.y;

		result.c1 = c1 - other.z;
		result.c2 = c2 - other.z;
		result.c3 = c3 - other.z;
		result.c4 = c4 - other.z;

		result.d1 = d1;
		result.d2 = d2;
		result.d3 = d3;
		result.d4 = d4;
		// result = (*this) - other
		return result;
	}

	Point& PointmultiplyMatrix(const struct Point& m)
	{
		Point result;

		// Point와 Matrix의 곱셈 연산
		result.x = a1 * m.x + b1 * m.y + c1 * m.z + d1 * m.w;
		result.y = a2 * m.x + b2 * m.y + c2 * m.z + d2 * m.w;
		result.z = a3 * m.x + b3 * m.y + c3 * m.z + d3 * m.w;
		result.w = a4 * m.x + b4 * m.y + c4 * m.z + d4 * m.w;

		return result;
	}

	Point PointPlusMatrix(const struct Point& m)
	{
		// Point와 Matrix의 덧셈 연산
		Point result((d1 * m.w) + (a1 * m.x) + (b1 * m.y) + (c1 * m.z)
			, (d2 * m.w) + (a2 * m.x) + (b2 * m.y) + (c2 * m.z)
			, (d3 * m.w) + (a3 * m.x) + (b3 * m.y) + (c3 * m.z)
			, (d4 * m.w) + (a4 * m.x) + (b4 * m.y) + (c4 * m.z));
		return result;
	}

	Point PointMinusMatrix(const struct Point& m)
	{
		// Point와 Matrix의 뺄셈 연산
		Point result((d1 * m.w) - (a1 * m.x) - (b1 * m.y) - (c1 * m.z)
			, (d2 * m.w) - (a2 * m.x) - (b2 * m.y) - (c2 * m.z)
			, (d3 * m.w) - (a3 * m.x) - (b3 * m.y) - (c3 * m.z)
			, (d4 * m.w) - (a4 * m.x) - (b4 * m.y) - (c4 * m.z));

		return result;
	}

	bool inverseMatrix()
	{
		float det = ((a4 * b3 * c2 * d1) - (a3 * b4 * c2 * d1) - (a4 * b2 * c3 * d1) + (a2 * b4 * c3 * d1) + (a3 * b2 * c4 * d1)
			- (a2 * b3 * c4 * d1) - (a4 * b3 * c1 * d2) + (a3 * b4 * c1 * d2) + (a4 * b1 * c3 * d2) - (a1 * b4 * c3 * d2)
			- (a3 * b1 * c4 * d2) + (a1 * b3 * c4 * d2) + (a4 * b2 * c1 * d3) - (a2 * b4 * c1 * d3) - (a4 * b1 * c2 * d3)
			+ (a1 * b4 * c2 * d3) + (a2 * b1 * c4 * d3) - (a1 * b2 * c4 * d3) - (a3 * b2 * c1 * d4) + (a2 * b3 * c1 * d4)
			+ (a3 * b1 * c2 * d4) - (a1 * b3 * c2 * d4) - (a2 * b1 * c3 * d4) + (a1 * b2 * c3 * d4));

		if (std::abs(det) < 1e-6) {
			//std::cerr << "Determinant is close to zero. No unique solution exists." << std::endl;
			// 여기에서 오류 처리 또는 다른 조치를 취할 수 있습니다.
			return false;
		}

		Matrix inverse;

		inverse.a1 = ((b2 * c3 * d4) - (b3 * c2 * d4) - (b2 * c4 * d3) + (b4 * c2 * d3) + (b3 * c4 * d2) - (b4 * c3 * d2)) / det;
		inverse.a2 = ((a4 * c3 * d2) - (a3 * c4 * d2) - (a4 * c2 * d3) + (a2 * c4 * d3) + (a3 * c2 * d4) - (a2 * c3 * d4)) / det;
		inverse.a3 = ((a2 * b3 * d4) - (a3 * b2 * d4) - (a2 * b4 * d3) + (a4 * b2 * d3) * (a3 * b4 * d2) - (a4 * d3 * d2)) / det;
		inverse.a4 = (-(a2 * b3 * c4) + (a3 * b2 * c4) + (a2 * b4 * c3) - (a4 * b2 * c3) - (a3 * b4 * c2) + (a4 * b3 * c2)) / det;


		inverse.b1 = (-(b1 * c3 * d4) + (b3 * c1 * d4) + (b1 * c4 * d3) - (b4 * c1 * d3) - (b3 * c4 * d1) + (b4 * c3 * d1)) / det;
		inverse.b2 = ((a1 * c3 * d4) - (a3 * c1 * d4) - (a1 * c4 * d3) + (a4 * c1 * d3) + (a3 * c4 * d1) - (a4 * c3 * d1)) / det;
		inverse.b3 = (-(a1 * b3 * d4) + (a3 * b1 * d4) - (a1 * b1 * d3) - (a3 * b1 * d3) - (a3 * b4 * d1) + (a4 * b3 * d1)) / det;
		inverse.b4 = ((a1 * b3 * c4) - (a3 * b1 * c4) - (a1 * b4 * c3) + (a4 * b1 * c3) + (a3 * b4 * c1) - (a4 * b3 * c1)) / det;

		inverse.c1 = ((b1 * c2 * d4) - (b2 * c1 * d4) - (b1 * c4 * d2) + (b4 * c1 * d2) + (b2 * c4 * d1) - (b4 * c2 * d1)) / det;
		inverse.c2 = (-(a1 * c2 * d4) + (a2 * c1 * d4) + (a1 * c4 * d2) - (a4 * c1 * d2) - (a2 * c4 * d1) + (a4 * c2 * d1)) / det;
		inverse.c3 = ((a1 * b2 * d4) - (a2 * b1 * d4) - (a1 * b4 * d2) + (a4 * b1 * d2) + (a2 * b4 * d1) - (a4 * b2 * d1)) / det;
		inverse.c4 = (-(a1 * b2 * c4) + (a2 * b1 * c4) + (a1 * b4 * c2) - (a4 * b1 * c2) - (a2 * b4 * c1) + (a4 * b2 * c1)) / det;

		inverse.d1 = (-(b1 * c2 * d3) + (b2 * c1 * d3) + (b1 * c3 * d2) - (b3 * c1 * d2) - (b2 * c3 * d1) + (b3 * c2 * d1)) / det;
		inverse.d2 = ((a1 * c2 * d3) - (a2 * c1 * d3) - (a1 * c3 * d2) + (a3 * c1 * d2) + (a2 * c3 * d1) - (a3 * c2 * d1)) / det;
		inverse.d3 = (-(a1 * b2 * d3) + (a2 * b1 * d3) + (a1 * b3 * d2) - (a3 * b1 * d2) - (a2 * b3 * d1) + (a3 * b2 * d1)) / det;
		inverse.d4 = ((a1 * b2 * c3) - (a2 * b1 * c3) - (a1 * b3 * c2) + (a3 * b1 * c2) + (a2 * b3 * c1) - (a3 * b2 * c1)) / det;

		*this = inverse;

		return true;
	}

	float GetDet()
	{
		float det = ((a4 * b3 * c2 * d1) - (a3 * b4 * c2 * d1) - (a4 * b2 * c3 * d1) + (a2 * b4 * c3 * d1) + (a3 * b2 * c4 * d1)
			- (a2 * b3 * c4 * d1) - (a4 * b3 * c1 * d2) + (a3 * b4 * c1 * d2) + (a4 * b1 * c3 * d2) - (a1 * b4 * c3 * d2)
			- (a3 * b1 * c4 * d2) + (a1 * b3 * c4 * d2) + (a4 * b2 * c1 * d3) - (a2 * b4 * c1 * d3) - (a4 * b1 * c2 * d3)
			+ (a1 * b4 * c2 * d3) + (a2 * b1 * c4 * d3) - (a1 * b2 * c4 * d3) - (a3 * b2 * c1 * d4) + (a2 * b3 * c1 * d4)
			+ (a3 * b1 * c2 * d4) - (a1 * b3 * c2 * d4) - (a2 * b1 * c3 * d4) + (a1 * b2 * c3 * d4));
		return det;
	}

	Matrix Transposed() const {
		Matrix t;
		t.a1 = a1;
		t.a2 = b1;
		t.a3 = c1;
		t.a4 = d1;

		t.b1 = a2;
		t.b2 = b2;
		t.b3 = c2;
		t.b4 = d2;
		
		t.c1 = a3;
		t.c2 = b3;
		t.c3 = c3;
		t.c4 = d3;
		
		t.d1 = a4;
		t.d2 = b4;
		t.d3 = c4;
		t.d4 = d4;
		
		return t;
	}

	float a1, a2, a3, a4;
	float b1, b2, b3, b4;
	float c1, c2, c3, c4;
	float d1, d2, d3, d4;
};

struct Quaternion {
	float x, y, z, w;

	Quaternion() : x(0), y(0), z(0), w(1) {}
	Quaternion(float _x, float _y, float _z, float _w)
		: x(_x), y(_y), z(_z), w(_w) {}

	// axis 는 단위벡터이어야 합니다.
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

		// 순서: ZYX (Yaw-Pitch-Roll 순서)
		return Quaternion(
			sx * cy * cz - cx * sy * sz,  // x
			cx * sy * cz + sx * cy * sz,  // y
			cx * cy * sz - sx * sy * cz,  // z
			cx * cy * cz + sx * sy * sz   // w
		);
	}

	// 쿼터니언 곱 (this 먼저, 그 다음 q)
	Quaternion operator*(const Quaternion& q) const {
		return Quaternion(
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w,
			w * q.w - x * q.x - y * q.y - z * q.z
		);
	}

	// 길이 1로 정규화
	void Normalize() {
		float len = sqrtf(x * x + y * y + z * z + w * w);
		if (len > 0.0f) {
			x /= len; y /= len; z /= len; w /= len;
		}
	}

	// Row-major × Column-vector 용 4×4 회전 행렬으로 변환
	Matrix ToMatrix() const {
		float xx = x * x, yy = y * y, zz = z * z;
		float xy = x * y, xz = x * z, yz = y * z;
		float wx = w * x, wy = w * y, wz = w * z;

		return Matrix{
			//  m11             m12             m13         m14
			1 - 2 * yy - 2 * zz,    2 * xy + 2 * wz,      2 * xz - 2 * wy,      0,
			//  m21             m22             m23         m24
			2 * xy - 2 * wz,      1 - 2 * xx - 2 * zz,    2 * yz + 2 * wx,      0,
			//  m31             m32             m33         m34
			2 * xz + 2 * wy,      2 * yz - 2 * wx,      1 - 2 * xx - 2 * yy,    0,
			//  m41             m42             m43         m44
			0,              0,              0,              1
		};
	}
};