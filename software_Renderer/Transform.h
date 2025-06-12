///
/// DX기준에 맞춰서 한다. 행벡터 기준으로 만들것.
/// 
/// 
#pragma once
#include "Matrix.hpp"

#define PI(x) x *= 0.0174533

class Transform
{
public:
	Transform();
	~Transform();
public:
	void Update(); //매 프레임마다 업데이트 되는 함수. 이 함수 내부에서 TRS연산이 진행된다.
	void SetParent(Transform* parentTransform); // 해당 트랜스폼의 부모를 결정.
	void SetPosition(float posx, float posy, float posz);	// position값을 설정. 직접 변환행렬을 계산하는것이 아님에 유의
	void SetRotation(float rotx, float roty, float rotz);	// rotation값을 설정. 라디안이 아닌 각도를 기반. 직접 변환행렬을 계산하는것이 아님에 유의
	void SetScale(float scalx, float scaly, float scalz);	// scale값을 설정. 직접 변환행렬을 계산하는것이 아님에 유의
	Matrix GetTransformMatrix();

public:
	Point PointmultiplyMatrix(const struct Point& m);
	Point PointPlusMatrix(const struct Point& m);
	Point PointMinusMatrix(const struct Point& m);
private:
	Matrix TransformHierarchy(); // 만약 나에게 부모가 있다면 부모의 matrix를 가져와서 연산하도록 하는 함수. 재귀로 구성
	Matrix Translate();
	Matrix Rotation();
	Matrix Scale();
	Matrix RotationQ();
private:
	float posX, posY, posZ;
	float rotX, rotY, rotZ;
	float scalX, scalY, scalZ;

	Matrix transformMatrix;
	Transform* parent;
};
