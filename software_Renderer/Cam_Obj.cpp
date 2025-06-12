#include "Cam_Obj.h"

Cam_Obj::Cam_Obj()
{
	type = RenderType::NONE;

	pcam = new Camera();
	Initialize();
}

Cam_Obj::~Cam_Obj()
{
	delete pcam;
}

void Cam_Obj::Update()
{
	Point pos, up, f, tar;

	transform.Update();
	//ExtractCameraParams(transform.GetTransformMatrix(), pos, tar, up);
	//pcam->LookAt(pos, tar, up);
	pcam->Update();
}

void Cam_Obj::ExtractCameraParams(Matrix mat, Point& pos, Point& tar, Point& up)
{
	// 행벡터 기준, m[행][열]
	pos.x = mat.m[3][0];
	pos.y = mat.m[3][1];
	pos.z = mat.m[3][2];

	up.x = mat.m[1][0];
	up.y = mat.m[1][1];
	up.z = mat.m[1][2];

	Point forward;
	forward.x = mat.m[2][0];
	forward.y = mat.m[2][1];
	forward.z = mat.m[2][2];

	tar.x = pos.x + forward.x;
	tar.y = pos.y + forward.y;
	tar.z = pos.z + forward.z;
}