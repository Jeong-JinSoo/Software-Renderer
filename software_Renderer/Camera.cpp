#include "Camera.h"
#include <DirectXMath.h>

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Update(bool isobj)
{
	if (isobj)
	{
		
	}

	else
	{
		ClacViewMatrix();
	}

	ClacProjectionMatrix();
}

void Camera::LookAt(Point camPos, Point target, Point worldUp)
{
	eye = camPos;
	at = target;
	up = worldUp;
}

void Camera::CreatePerspectiveFrustum(float fov, float aspect, float near, float far)
{
	this->_fovy = fov;
	this->_aspect = aspect;
	this->_near = near;
	this->_far = far;
}

void Camera::SetViewPort(float hight, float length)
{
	windowHight = hight;
	windowLength = length;
	float min = 0.0;	//depthÀÇ ÃÖ¼Ú°ª
	float max = 1.0;	//depthÀÇ ÃÖ´ñ°ª
	float x = 0;		//left
	float y = 0;		//top

	Matrix result;

	result = {
		  length / 2,			  0,		  0,			0
	,			 0,	 -(hight / 2),	 	  0,			0
	, 			 0,			  0,	max - min,			0
	, x + (length / 2),	y + (hight / 2),		min,			1
	};

	viewPort = result;
}

void Camera::ClacProjectionMatrix()
{
	Matrix result;
	float yScale = 1.0f / tanf(_fovy / 2.0f);
	float xScale = yScale / _aspect;
	float zn = _near;
	float zf = _far;

	result = {
		xScale,			0,						 0,        0,
			 0,    yScale,						 0,        0,
			 0,		    0,			zf / (zf - zn),		   1,
			 0,			0,	  -zn * zf / (zf - zn),		   0
	};

	proj = result;
}
                   

void Camera::ClacViewMatrix()
{
	Matrix result;

   	Point n = Point::Normalize(at-eye); // Z
   	Point u = Point::Normalize(Point::Cross(n,up)); // Y
   	Point v = Point::Cross(u, n); // X

	result = {
						 u.x,					   v.x,						   n.x,		0
	,					 u.y,					   v.y,						   n.y,		0
	,					 u.z,					   v.z,						   n.z,		0
	, -(Point::Dot(u , eye)),	-(Point::Dot(v , eye)),		-(Point::Dot(n , eye)),		1
	};
	view = result;
	return;
}

void Camera::ClacViewMatrix_Obj(Transform world)
{
	Matrix result = world.GetTransformMatrix();
	//result.inverseMatrix();
	view = result;
}

Matrix Camera::GetView()
{
	return view;
}

Matrix Camera::GetProj()
{
	return proj;
}

Matrix Camera::GetViewPort()
{
	return viewPort;
}

Point Camera::GetViewPortLH()
{
	Point result;
	result.x = viewPort.a1 * 2.0f;
	result.y = viewPort.b2 * 2.0f;

	return result;
}