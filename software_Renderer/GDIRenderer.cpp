#include "GDIRenderer.h"
#include "BMP_Printer.hpp"

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
	objects.push_back(new Cube());

	cam.pcam->LookAt(Point(0, 5, -10), Point(0, 0, 0), Point(0, 1, 0));
	//cam.transform.SetPosition(0, 5, 10);
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

	objects[3]->transform.SetScale(0.1f, 0.1f, 0.1f);

	// axis_Widget
	objects[2]->transform.SetPosition(-3, 0, 0);

	/// 레스터 라이저를 더하면서 새로 추가된 프레임 버퍼&뎁스 버퍼

	// BITMAPINFO
	ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
	BITMAPINFOHEADER& hdr = bitmapInfo.bmiHeader;
	hdr.biSize = sizeof(BITMAPINFOHEADER);
	hdr.biWidth = screenWidth;
	hdr.biHeight = -screenheight;			// 음수로 하면 top-down DIB
	hdr.biPlanes = 1;
	hdr.biBitCount = 32;			// 32비트 픽셀
	hdr.biCompression = BI_RGB;		// 비압축 RGB
	hdr.biSizeImage = screenWidth * screenheight * 4;	// (optional: 0 도 가능)

	// 화면 크기만큼 버퍼 생성.
	/// 화면 바뀌면 다시 계산 해줘야 함!
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
	Inputer();

	//cube.transform.SetPosition(posx, posy, 0);
	objects[0]->transform.SetPosition(controlvaluex, controlvaluey, controlvaluez);
	//objects[0]->transform.SetScale(controlvaluex, controlvaluey, controlvaluez);
	objects[0]->transform.SetRotation(controlrot1, controlrot2, controlrot3);

	//cam.pcam->LookAt(Point(controlvaluex, controlvaluey, controlvaluez),Point(),Point(0,1,0));
	//cam.transform.SetPosition(controlvaluex, controlvaluey, controlvaluez);
	//cam.transform.SetRotation(controlrot1, controlrot2, controlrot3);
	

	cam.Update();
	for (auto obj : objects)
	{
		obj->Update();
	}

	for (auto obj : objects)
	{
		// 그리는게 면이라면
		if (obj->type == RenderType::Face)
		{
			Rasterizer::RasterizerTriangle(*obj, frameBuffer, depthBuffer, screenWidth, screenheight);

			BMP_Printer::SaveFrameBufferAsBMP(frameBuffer, screenWidth, screenheight, "output_color.bmp");
			BMP_Printer::SaveDepthBufferAsBMP(depthBuffer, screenWidth, screenheight, "output_depth.bmp");
		}

		// 그리는게 선이라면
		else if (obj->type == RenderType::Line)
		{
			//Rasterizer::RasterizerLine(*obj, screenWidth, screenheight, frameBuffer, depthBuffer);
		}
	}

	// 디버깅용 그라데이션
	//int w = screenWeight, h = screenheight;
	//for (int y = 0; y < h; ++y) {
	//	for (int x = 0; x < w; ++x) {
	//		// x축은 빨강, y축은 초록 농도
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

	/// 레스터 라이징 빼고 지오메트리 파이프라인 점검용
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
	DrawTextM(10, 30, L"pos : x:%f, y:%f, z:%f", controlvaluex, controlvaluey, controlvaluez);
	DrawTextM(10, 50, L"rot : x:%f, y:%f, z:%f", controlrot1, controlrot2, controlrot3);
}

void GDIRenderer::BeginRender()
{
	//PatBlt(memdc, 0, 0, rect.right, rect.bottom, BLACKNESS);

	// 레스터 라이저 추가 이후

	// 1. 색 버퍼를 배경색으로 채우기 (검정 : 0xFF000000
	std::fill(frameBuffer.begin(), frameBuffer.end(), 0xFF000000u);

	// 2. 깊이 버퍼를 무한대(또는 매우 큰 값)로 채우기
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
	// 1) 현재 클라이언트 영역을 다시 읽어 오고
	GetClientRect(*phwnd, &rect);
	screenWidth = rect.right - rect.left;
	screenheight = rect.bottom - rect.top;
	int w = rect.right, h = rect.bottom;

	// 2) 카메라 재설정
	float fov = 45.0f * (3.14159265f / 180.0f);
	cam.pcam->CreatePerspectiveFrustum(fov, (float)screenheight / (float)screenWidth, 1, 1000);
	cam.pcam->SetViewPort(screenheight, screenWidth);


	// 2) 기존 비트맵 해제
	SelectObject(memdc, oldBitmap);
	DeleteObject(memBitmap);

	// 3) 새로운 크기로 메모리 비트맵 생성 & 선택
	memBitmap = CreateCompatibleBitmap(GetDC(*phwnd), w, h);
	oldBitmap = (HBITMAP)SelectObject(memdc, memBitmap);

	// 4) 버퍼 크기 & BITMAPINFO 재설정
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
	// 배경 모드를 TRANSPARENT로 설정
	SetBkMode(memdc, TRANSPARENT);

	// 배경 브러시를 선택
	HBRUSH oldBrush = (HBRUSH)SelectObject(memdc, backgroundBrush);
	SetTextColor(memdc, RGB(255, 255, 255));

	//////가변인자를 적용하여 문자열을 구성하는 부분//////
	wchar_t buffer[1024]; // 문자열을 저장할 버퍼
	va_list args;
	va_start(args, format);

	// 서식 문자열과 가변 인자를 사용하여 문자열 생성
	vswprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), format, args);

	va_end(args);
	///////////////////////////////////////////////////

	// TextOut 함수를 사용하여 문자열을 화면에 출력
	TextOut(memdc, xpos, ypos, buffer, lstrlen(buffer));

	// 이전 브러시로 선택
	SelectObject(memdc, oldBrush);

	// 배경 모드를 다시 OPAQUE로 설정 (기본값)
	SetBkMode(memdc, OPAQUE);
}

void GDIRenderer::Inputer()
{
	float deltatime = 0.0167;
	float speed = 10 * deltatime;
	input->Update(deltatime);

	if (input->KeyInput(KEY::Q))
	{
		controlvaluey += speed;
	}
	else if (input->KeyInput(KEY::E))
	{
		controlvaluey -= speed;
	}

	if (input->KeyInput(KEY::A))
	{
		controlvaluex -= speed;
	}
	else if (input->KeyInput(KEY::D))
	{
		controlvaluex += speed;
	}

	if (input->KeyInput(KEY::W))
	{
		controlvaluez += speed;
	}
	else if (input->KeyInput(KEY::S))
	{
		controlvaluez -= speed;
	}

	// x축 (상하회전)
	if (input->KeyInput(KEY::UP))
	{
		controlrot1 += 1;
	}
	else if (input->KeyInput(KEY::DOWN))
	{
		controlrot1 -= 1;
	}

	// y축 (회전)
	if (input->KeyInput(KEY::LEFT))
	{
		controlrot2 += 1;
	}
	else if (input->KeyInput(KEY::RIGHT))
	{
		controlrot2 -= 1;
	}

	// z축 (좌우 회전)
	if (input->KeyInput(KEY::HOME))
	{
		controlrot3 += 1;
	}
	else if (input->KeyInput(KEY::END))
	{
		controlrot3 -= 1;
	}
}
