#pragma once
#include <windows.h>
#include <wchar.h>
#include <vector>

#include "testdirectinput.h"
#include "Camera.h"
#include "Triangle.h"			// 트라이앵글
#include "Rasterizer.h"			// 레스터라이저

#include "Cube.h"
#include "Axis_Widget.h"
#include "Cam_Obj.h"
#include "Grid.h"

class GDIRenderer
{
public:
	GDIRenderer(HWND* hwnd);
	~GDIRenderer();
public:
	void Initialize();
	void Render();
	void BeginRender();
	void EndRender();
	void Finalize();

	void OnResize();

private:
	void DrawLine(float sposx, float sposy, float eposx, float eposy, _In_ COLORREF color);
	void DrawLine(Point spos, Point epos, _In_ COLORREF color);
	void DrawDot(float posx, float posy, _In_ COLORREF color);
	void DrawTextM(float xpos, float ypos, const wchar_t* text, ...);

	// 인풋이 렌더함수에 있는건 이상한거지만 일단 오브젝트 관리용
	// update가 렌더에 종속되어 있기도 하고 일단 구현이 먼저니까 일단 두고
	/// 나중에 꼭 챙겨서 옮겨놔라!
	void Inputer();

private:
	HWND* phwnd;

	HDC memdc;
	RECT rect;

	HPEN hMyPen;
	HPEN hOldPen;
	HBRUSH backgroundBrush;

	HBITMAP memBitmap;
	HBITMAP oldBitmap;

	std::vector<RenderObject*> objects;

	Cam_Obj cam;

	InputSystem* input;

	// 레스터라이저를 더하면서 새로 추가된 프레임 버퍼&뎁스 버퍼
	
	BITMAPINFO bitmapInfo;	// DIB(디바이스 독립 비트맵) 정보를 담는 구조체
							// 내부에 BITMAPINFOHEADER가 있고 8-bit이하 컬러 팔레트를 바로
							// 뒤에 붙일 수 있는 형태. 32-bit 컬러로 화면을 뿌릴 때는
							// 팔레트가 필요 없으므로 헤더만 설정
	std::vector<uint32_t> frameBuffer;	// 화면 크기만큼의 픽셀 색상 버퍼 (ARGB32)
	std::vector<float> depthBuffer;		// 화면 크기 만큼의 깊이 버퍼 (float)


	int screenWidth;
	int screenheight;
};

