#include <Windows.h>
#include <d2d1.h>

#include "GDIRenderer.h"
#include "testdirectinput.h"

GDIRenderer* gdiRenderer;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int windowsizeL = 680;
int windowsizeH = 680;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	///(내가 쓸 윈도우를) 등록
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;					///윈도우 프로시저
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 3);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"WindowClass";
	wcex.hIconSm = NULL;

	RegisterClassExW(&wcex);

	///(그 윈도우를) 생성
	HWND hWnd = CreateWindowW(L"WindowClass", L"Program Title", WS_OVERLAPPEDWINDOW,
		0, 0, windowsizeL, windowsizeH, nullptr, nullptr, hInstance, nullptr);

	//윈도우 핸들이 가장 중요 (이걸 기준으로 그림을 그려)
	if (!hWnd)
	{
		return FALSE;
	}

	gdiRenderer = new GDIRenderer(&hWnd);		// 소프트웨어 렌더러를 생성
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	gdiRenderer->Initialize();
	gdiRenderer->OnResize();

	InputSystem* cinput = InputSystem::GetInstance();	// 입력 시스템 생성(싱글턴방식)
	cinput->Initialize(hInstance, hWnd, 680, 680);		// 입력 시스템 초기화


	///무한루프(안에 메시지 루프와 게임 루프등이 포함됨)

	///메세지 루프
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)break;
			DispatchMessage(&msg);
		}
		else
		{
			gdiRenderer->BeginRender();


			gdiRenderer->Render();
			gdiRenderer->EndRender();
		}
	}

	return 0;
}

extern float controlvaluex;
extern float controlvaluey;
extern float controlvaluez;

extern float controlrot1;
extern float controlrot2;
extern float controlrot3;

extern bool convertcam;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_KEYDOWN:
			if (wParam == VK_UP)
			{
				controlvaluex -= 5;
			}
			if (wParam == VK_DOWN)
			{
				controlvaluex += 5;
			}
			if (wParam == VK_LEFT)
			{
				controlvaluey -= 5;
			}
			if (wParam == VK_RIGHT)
			{
				controlvaluey += 5;
			}
			if (wParam == VK_END)
			{
				controlvaluez -= 5;
			}
			if (wParam == VK_HOME)
			{
				controlvaluez += 5;
			}
			if (wParam == VK_SPACE)
			{
				convertcam = ((convertcam == false) ? true : false);
			}
				break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}