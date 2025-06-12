#pragma once
#include "Transform.h"

class Camera
{
public:
	Camera();
	~Camera();
public:
	void Update();

	void LookAt(Point camPos, Point target, Point worldUp);	// 카메라 기본 세팅

	void CreatePerspectiveFrustum(float fov, float aspect, float near, float far_);	//절두체 세팅
	void SetViewPort(float hight, float length);

	void ClacViewMatrix();
	void ClacProjectionMatrix();

	Matrix GetView();
	Matrix GetProj();
	Matrix GetViewPort();
	Point GetViewPortLH();

private:
	Point eye;
	Point at;
	Point up;

	float _fovy;		//시야각
	float _aspect;		//종횡비
	float _near;		//전방평면까지의 거리
	float _far;			//후방평면까지의거리

	float windowHight;
	float windowLength;
	
	Matrix view;
	Matrix proj;
	Matrix viewPort;
};

