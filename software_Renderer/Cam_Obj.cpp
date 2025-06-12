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
	//pcam->ClacViewMatrix_Obj(transform);
	pcam->Update(false);
}
