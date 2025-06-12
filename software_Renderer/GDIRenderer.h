#pragma once
#include <windows.h>
#include <wchar.h>
#include <vector>

#include "testdirectinput.h"
#include "Camera.h"
#include "Triangle.h"			// Ʈ���̾ޱ�
#include "Rasterizer.h"			// �����Ͷ�����

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

	// ��ǲ�� �����Լ��� �ִ°� �̻��Ѱ����� �ϴ� ������Ʈ ������
	// update�� ������ ���ӵǾ� �ֱ⵵ �ϰ� �ϴ� ������ �����ϱ� �ϴ� �ΰ�
	/// ���߿� �� ì�ܼ� �Űܳ���!
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

	// �����Ͷ������� ���ϸ鼭 ���� �߰��� ������ ����&���� ����
	
	BITMAPINFO bitmapInfo;	// DIB(����̽� ���� ��Ʈ��) ������ ��� ����ü
							// ���ο� BITMAPINFOHEADER�� �ְ� 8-bit���� �÷� �ȷ�Ʈ�� �ٷ�
							// �ڿ� ���� �� �ִ� ����. 32-bit �÷��� ȭ���� �Ѹ� ����
							// �ȷ�Ʈ�� �ʿ� �����Ƿ� ����� ����
	std::vector<uint32_t> frameBuffer;	// ȭ�� ũ�⸸ŭ�� �ȼ� ���� ���� (ARGB32)
	std::vector<float> depthBuffer;		// ȭ�� ũ�� ��ŭ�� ���� ���� (float)


	int screenWidth;
	int screenheight;
};

