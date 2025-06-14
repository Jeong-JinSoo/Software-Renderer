#pragma once
#include "RenderObject.h"
#include "Camera.h"

class Cam_Obj : public RenderObject
{
public:
	Cam_Obj();
	~Cam_Obj();

	virtual void Update() override;
	void ExtractCameraParams(Matrix mat, Point& pos, Point& tar, Point& up);
};

