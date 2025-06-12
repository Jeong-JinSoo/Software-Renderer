#include "GDIRenderer.h"

extern int windowsizeL;
extern int windowsizeH;

GDIRenderer::GDIRenderer(HWND* hwnd)
{
	phwnd = hwnd;
	memdc = CreateCompatibleDC(GetDC(*phwnd));
	GetClientRect(*phwnd, &rect);
	input = InputSystem::GetInstance();
}

GDIRenderer::~GDIRenderer()
{

}

void GDIRenderer::Initialize()
{
	memBitmap = CreateCompatibleBitmap(memdc, rect.right, rect.bottom);
	oldBitmap = (HBITMAP)SelectObject(memdc, memBitmap);
	FillRect(memdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

	backgroundBrush = CreateSolidBrush(RGB(0, 0, 0));

	objects.push_back(new Cube());
	objects.push_back(new Grid());
	objects.push_back(new Axis_Widget());

	cam.pcam->LookAt(Point(0, 5, 10), Point(0, 0, 0), Point(0, 1, 0));
	//cam.transform.SetPosition(0, 1, 5);
	//cam.transform.SetRotation(0, 0, 0);

	for (auto obj : objects)
	{
		obj->transform.SetScale(1, 1, 1);
		obj->transform.SetPosition(0, 0, 0);
		obj->transform.SetRotation(0, 0, 0);
		obj->pcam = cam.pcam;
	}
	//cube
	objects[0]->transform.SetPosition(0, 0, 0);

	// axis_Widget
	objects[2]->transform.SetPosition(-4, 0, 0);

	/// ������ �������� ���ϸ鼭 ���� �߰��� ������ ����&���� ����

	// BITMAPINFO
	ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
	BITMAPINFOHEADER& hdr = bitmapInfo.bmiHeader;
	hdr.biSize = sizeof(BITMAPINFOHEADER);
	hdr.biWidth = screenWidth;
	hdr.biHeight = -screenheight;			// ������ �ϸ� top-down DIB
	hdr.biPlanes = 1;
	hdr.biBitCount = 32;			// 32��Ʈ �ȼ�
	hdr.biCompression = BI_RGB;		// ����� RGB
	hdr.biSizeImage = screenWidth * screenheight * 4;	// (optional: 0 �� ����)

	// ȭ�� ũ�⸸ŭ ���� ����.
	/// ȭ�� �ٲ�� �ٽ� ��� ����� ��!
	int pixelCount = screenWidth * screenheight;
	frameBuffer.resize(pixelCount);
	depthBuffer.resize(pixelCount);
}

float posx = 0;
float posy = 0;
float posz = 0;

float controlrot1 = 0;
float controlrot2 = 0;
float controlrot3 = 0;

float controlvaluex = 0;
float controlvaluey = 0;
float controlvaluez = 0;

bool convertcam = true;

void GDIRenderer::Render()
{
	input->Update(0);

	//cube.transform.SetPosition(posx, posy, 0);
	//cube.transform.SetPosition(controlvaluex, controlvaluey, controlvaluez);
	objects[0]->transform.SetRotation(controlvaluex, controlvaluey, controlvaluez);

	//cam.pcam->LookAt(Point(controlvaluex, controlvaluey, controlvaluez),Point(),Point(0,1,0));
	//cam.transform.SetRotation(controlvaluex, controlvaluey, controlvaluez);
	

	cam.Update();
	for (auto obj : objects)
	{
		obj->Update();
	}

	for (auto obj : objects)
	{
		// �׸��°� ���̶��
		if (obj->type == RenderType::Face)
		{

			// ȭ�鿡 �׸� �ﰢ�� ��� �����
			std::vector<Triangle> tris;
			tris.reserve(12);	// ť���� �ﰢ���� 12��

			for (int ti = 0; ti < 12; ++ti)
			{
				Triangle tri;
				for (int k = 0; k < 3; ++k)
				{
					int index = obj->ReturnIndex(ti * 3 + k);

					Point p = obj->finalVertex[index].vPoint;
					Point n = obj->finalVertex[index].vNormal;
					tri.v[k].x = p.x;
					tri.v[k].y = p.y;
					tri.v[k].z = p.z;
					tri.v[k].nx = n.x;
					tri.v[k].ny = n.y;
					tri.v[k].nz = n.z;
					tri.v[k].wInv = obj->GetPrevClipW(index);
					tri.v[k].u = obj->vertecies[index].u;
					tri.v[k].v = obj->vertecies[index].v;
				}
				tris.push_back(tri);
			}

			// (���� DrawLine ��� ���) �����Ͷ����� ȣ��
			//Clear(frameBuffer/depthBuffer) ��
			for (auto& tri : tris)
			{
				Rasterizer::RasterizerTriangle(tri, frameBuffer, depthBuffer, screenWidth, screenheight);
			}
		}

		// �׸��°� ���̶��
		else if (obj->type == RenderType::Line)
		{
			for (int i = 0; i < obj->indices.size(); i += 2)
			{
				Point v0 = obj->finalVertex[obj->indices[i]].vPoint;
				Point v1 = obj->finalVertex[obj->indices[i + 1]].vPoint;
				Rasterizer::RasterizerLine(screenWidth, screenheight, v0, v1, frameBuffer, depthBuffer, obj->finalVertex[obj->indices[i + 1]].color);
			}
		}
	}

	// ������ �׶��̼�
	//int w = screenWeight, h = screenheight;
	//for (int y = 0; y < h; ++y) {
	//	for (int x = 0; x < w; ++x) {
	//		// x���� ����, y���� �ʷ� ��
	//		uint8_t r = uint8_t(255 * x / (w - 1));
	//		uint8_t g = uint8_t(255 * y / (h - 1));
	//		frameBuffer[y * w + x] = (0xFFu << 24) | (r << 16) | (g << 8);
	//	}
	//}
	
	StretchDIBits(memdc,
		0, 0, screenWidth, screenheight,
		0, 0, screenWidth, screenheight,
		frameBuffer.data(),
		&bitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY);

	/// ������ ����¡ ���� ������Ʈ�� ���������� ���˿�
	//Point dot[20];
	
	//for (int i = 0; i < 20; i++)
	//{
	//	dot[i] = grid.finalVertex[i].vPoint;
	//}
	
	//for (int i = 0; i < 20; i+=2)
	//{
	//	DrawLine(dot[grid.ReturnIndex(i)], dot[grid.ReturnIndex(i + 1)], RGB(255, 255, 255));
	//}
	
	Point viewport = cam.pcam->GetViewPortLH();
	DrawTextM(10, 10, L"Viewport x : %f, y %f", viewport.x, viewport.y);
	DrawTextM(10, 30, L"control value : x:%f, y:%f, z:%f", controlvaluex, controlvaluey, controlvaluez);
}

void GDIRenderer::BeginRender()
{
	//PatBlt(memdc, 0, 0, rect.right, rect.bottom, BLACKNESS);

	// ������ ������ �߰� ����

	// 1. �� ���۸� �������� ä��� (���� : 0xFF000000
	std::fill(frameBuffer.begin(), frameBuffer.end(), 0xFF000000u);

	// 2. ���� ���۸� ���Ѵ�(�Ǵ� �ſ� ū ��)�� ä���
	std::fill(depthBuffer.begin(), depthBuffer.end(), FLT_MAX);
}

void GDIRenderer::EndRender()
{
	BitBlt(GetDC(*phwnd), 0, 0, rect.right, rect.bottom, memdc, 0, 0, SRCCOPY);

}

void GDIRenderer::Finalize()
{
	DeleteObject(backgroundBrush);

	SelectObject(memdc, oldBitmap);
	DeleteObject(memBitmap);

	ReleaseDC(*phwnd, memdc);
}

void GDIRenderer::OnResize()
{
	// 1) ���� Ŭ���̾�Ʈ ������ �ٽ� �о� ����
	GetClientRect(*phwnd, &rect);
	screenWidth = rect.right - rect.left;
	screenheight = rect.bottom - rect.top;
	int w = rect.right, h = rect.bottom;

	// 2) ī�޶� �缳��
	float fov = 45.0f * (3.14159265f / 180.0f);
	cam.pcam->CreatePerspectiveFrustum(fov, (float)screenheight / (float)screenWidth, 1, 1000);
	cam.pcam->SetViewPort(screenheight, screenWidth);


	// 2) ���� ��Ʈ�� ����
	SelectObject(memdc, oldBitmap);
	DeleteObject(memBitmap);

	// 3) ���ο� ũ��� �޸� ��Ʈ�� ���� & ����
	memBitmap = CreateCompatibleBitmap(GetDC(*phwnd), w, h);
	oldBitmap = (HBITMAP)SelectObject(memdc, memBitmap);

	// 4) ���� ũ�� & BITMAPINFO �缳��
	frameBuffer.assign(w * h, 0xFF000000u);
	depthBuffer.assign(w * h, FLT_MAX);

	ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
	auto& hdr = bitmapInfo.bmiHeader;
	hdr.biSize = sizeof(hdr);
	hdr.biWidth = w;
	hdr.biHeight = -h;       // top-down
	hdr.biPlanes = 1;
	hdr.biBitCount = 32;
	hdr.biCompression = BI_RGB;
}

void GDIRenderer::DrawLine(float sposx, float sposy, float eposx, float eposy, _In_ COLORREF color)
{
	hMyPen = CreatePen(PS_SOLID, 2, color);
	hOldPen = (HPEN)SelectObject(memdc, hMyPen);

	MoveToEx(memdc, sposx, sposy, NULL);
	LineTo(memdc, eposx, eposy);

	SelectObject(memdc, hOldPen);
	DeleteObject(hMyPen);
}

void GDIRenderer::DrawLine(Point spos, Point epos, _In_ COLORREF color)
{
	hMyPen = CreatePen(PS_SOLID, 2, color);
	hOldPen = (HPEN)SelectObject(memdc, hMyPen);

	MoveToEx(memdc, spos.x, spos.y, NULL);
	LineTo(memdc, epos.x, epos.y);

	SelectObject(memdc, hOldPen);
	DeleteObject(hMyPen);
}

void GDIRenderer::DrawDot(float posx, float posy, _In_ COLORREF color)
{
	hMyPen = CreatePen(PS_SOLID, 2, color);
	hOldPen = (HPEN)SelectObject(memdc, hMyPen);

	MoveToEx(memdc, posx, posy, NULL);
	LineTo(memdc, posx, posy);

	SelectObject(memdc, hOldPen);
	DeleteObject(hMyPen);
}

void GDIRenderer::DrawTextM(float xpos, float ypos, const wchar_t* format, ...)
{
	// ��� ��带 TRANSPARENT�� ����
	SetBkMode(memdc, TRANSPARENT);

	// ��� �귯�ø� ����
	HBRUSH oldBrush = (HBRUSH)SelectObject(memdc, backgroundBrush);
	SetTextColor(memdc, RGB(255, 255, 255));

	//////�������ڸ� �����Ͽ� ���ڿ��� �����ϴ� �κ�//////
	wchar_t buffer[1024]; // ���ڿ��� ������ ����
	va_list args;
	va_start(args, format);

	// ���� ���ڿ��� ���� ���ڸ� ����Ͽ� ���ڿ� ����
	vswprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), format, args);

	va_end(args);
	///////////////////////////////////////////////////

	// TextOut �Լ��� ����Ͽ� ���ڿ��� ȭ�鿡 ���
	TextOut(memdc, xpos, ypos, buffer, lstrlen(buffer));

	// ���� �귯�÷� ����
	SelectObject(memdc, oldBrush);

	// ��� ��带 �ٽ� OPAQUE�� ���� (�⺻��)
	SetBkMode(memdc, OPAQUE);
}
