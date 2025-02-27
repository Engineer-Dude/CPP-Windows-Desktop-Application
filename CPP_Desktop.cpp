// CPP_Desktop.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "CPP_Desktop.h"

// These are used for the processing of the png image.
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

// These are used for displaying the png image.
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// This is used for changing the color of the window background.
HBRUSH hbrWindowBackground;

// These are used for displaying the png image, including starting it up.
IWICStream* pStream = nullptr;
Gdiplus::GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;
Gdiplus::Status gdiplusStartupStatus = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// This is a forward declaration of the method used for loading the lightbulb png image.
void LoadLightbulbPng(HWND hwndParent);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (gdiplusStartupStatus != Gdiplus::Ok)
	{
		MessageBox(nullptr, TEXT("Failed to initialize GDI+"), TEXT("Error"), MB_OK);
		return -1;
	}

	// TODO: Place code here.


	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CPPDESKTOP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPPDESKTOP));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// This is for shutting Gdiplus
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPPDESKTOP));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CPPDESKTOP);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: {
		hbrWindowBackground = CreateSolidBrush(RGB(13, 79, 132));

		HWND hLabel = CreateWindow(
			TEXT("STATIC"),				// Predefined class for a label
			TEXT("Hello, world!"),		// Text to display
			WS_CHILD | WS_VISIBLE,		// Styles
			10, 10, 200, 20,			// Position and size
			hWnd,						// Parent window
			NULL,						// No menu
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL						// No additional parameters
		);

		HWND hwndButton = CreateWindow(
			TEXT("BUTTON"),											// Predefined class
			TEXT("Click Me"),										// Button text
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,	// Styles
			10,														// X-position
			40,														// Y-position
			100,													// Width
			30,														// Height
			hWnd,													// Parent window
			(HMENU)ID_BUTTON1,												// No menu
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL													// Pointer not needed
		);

		HWND hLabelTextBox = CreateWindow(
			TEXT("STATIC"),				// Predefined class for a label
			TEXT("Enter a value"),		// Text to display
			WS_CHILD | WS_VISIBLE,		// Styles
			10, 85, 100, 20,			// Position and size
			hWnd,						// Parent window
			NULL,						// No menu
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL						// No additional parameters
		);

		HWND hwndTextBox = CreateWindowEx(
			WS_EX_CLIENTEDGE,				// Extended window style
			TEXT("EDIT"),					// Predefined class
			NULL,							// No default text
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,	// Styles
			180,								// X-position
			80,								// Y-position
			200,							// Width
			30,								// Height
			hWnd,							// Parent window
			(HMENU)ID_TEXTBOX1,				// Control ID
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL							// Pointer not needed
		);

		LoadLightbulbPng(hWnd);

		break;
	}

	case WM_ERASEBKGND:
	{
		HDC hdc = (HDC)wParam;
		RECT rc;
		GetClientRect(hWnd, &rc);
		FillRect(hdc, &rc, hbrWindowBackground);
		return 1;
	}

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_TEXTBOX1:
			if (HIWORD(wParam) == EN_SETFOCUS)
			{
				MessageBox(hWnd, TEXT("Text Box 1 was pressed"), TEXT("Notification"), MB_OK);
			}
			break;
		case ID_BUTTON1:
			MessageBox(hWnd, TEXT("Button (click me) was pressed"), TEXT("Notification"), MB_OK);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// TODO: Add any drawing code that uses hdc here...
		Gdiplus::Graphics graphics(hdc);
		Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(pStream);
		graphics.DrawImage(pBitmap, 0, 0);
		delete pBitmap;

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void LoadLightbulbPng(HWND hwndParent)
{
	IWICImagingFactory* pFactory = nullptr;
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pFrame = nullptr;
	pStream = nullptr;
	HRSRC hResource = FindResource(hInst, MAKEINTRESOURCE(IDR_PNG_LIGHTBULB), TEXT("PNG"));

	if (hResource)
	{
		HGLOBAL hMemory = LoadResource(hInst, hResource);
		DWORD imageSize = SizeofResource(hInst, hResource);
		void* pResourceData = LockResource(hMemory);

		CoInitialize(nullptr);
		CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
		pFactory->CreateStream(&pStream);
		pStream->InitializeFromMemory(reinterpret_cast<BYTE*>(pResourceData), imageSize);
		pFactory->CreateDecoderFromStream(pStream, nullptr, WICDecodeMetadataCacheOnLoad, &pDecoder);
		pDecoder->GetFrame(0, &pFrame);
	}
	else
	{
		MessageBox(hwndParent, TEXT("Error finding resource"), TEXT("Error"), MB_OK);
	}
}