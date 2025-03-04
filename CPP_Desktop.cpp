// CPP_Desktop.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "CPP_Desktop.h"
#include "DeviceRegister.h"
#include <vector>

// These are used for the processing of the png image.
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

// These are used for displaying the png image.
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#include "ftd2xx.h"
#include <iostream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

std::vector<DeviceRegister> deviceRegisters;

// Brushes
HBRUSH hBrush = NULL;
HBRUSH hbrWindowBackground;
HBRUSH hbrRegisterArea;
HBRUSH hbrWhite;

HPEN hBlackPen;

HWND register_0_label;
HWND register_1_label;
HWND register_2_label;


// These are used for displaying the png image, including starting it up.
IWICStream* pStream = nullptr;
Gdiplus::GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;
Gdiplus::Status gdiplusStartupStatus = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void DisplayCheckBoxState(HWND hWnd, int controlId, std::string controlName);
HWND PlaceRegisterArea(HWND& hWnd, HDC hdc, DeviceRegister reg, int controls[], LONG& left, LONG& top, LONG& right, LONG& bottom);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void LoadLightbulbPng(HWND hwndParent);
void InitializeDeviceRegisters();

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

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CPPDESKTOP, szWindowClass, MAX_LOADSTRING);

	// Initialize the Device Registers
	InitializeDeviceRegisters();

	// ===========================

	FT_HANDLE ftHandle = NULL;
	FT_STATUS ftStatus;
	DWORD numberOfDevices;

	bool dc590bFound = true;

	ftStatus = FT_CreateDeviceInfoList(&numberOfDevices);
	if (ftStatus != FT_OK)
	{
		return 1;
	}

	FT_DEVICE_LIST_INFO_NODE* deviceInfo = new FT_DEVICE_LIST_INFO_NODE[numberOfDevices];

	ftStatus = FT_GetDeviceInfoList(deviceInfo, &numberOfDevices);
	if (ftStatus != FT_OK)
	{
		return 1;
	}

	if (numberOfDevices == 0)
	{
		dc590bFound = false;
	}
	else if (numberOfDevices > 1)
	{
		// Found more than expected
		return 1;
	}
	else
	{
		ftStatus = FT_Open(0, &ftHandle);
		if (ftStatus != FT_OK)
		{
			// Error
		}

		FT_SetBaudRate(ftHandle, 9600);
		FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
		FT_SetTimeouts(ftHandle, 1000, 1000);

		DWORD bytesWritten;
		char writeBuffer[] = "i\r";
		DWORD bytesRead;
		char readBuffer[256]{ 0 };

		ftStatus = FT_Write(ftHandle, writeBuffer, sizeof(writeBuffer) / sizeof(char), &bytesWritten);
		ftStatus = FT_Read(ftHandle, readBuffer, 41, &bytesRead);

		std::cout << "DC590B returned " << readBuffer << std::endl;
	}

	// ===========================

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPPDESKTOP));

	MSG msg;

	// 137 = 128 + 8 + 1 = 10001001 = 0x89
	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (dc590bFound == false)
		{
			SendMessage(msg.hwnd, WM_COMMAND, 0x01000089, NULL);
		}

		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	ftStatus = FT_Close(ftHandle);
	if (ftStatus != FT_OK)
	{
		// Error
	}

	// This is for shutting Gdiplus
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}

void InitializeDeviceRegisters()
{
	deviceRegisters.emplace_back("Register 0", 3);
	RegisterBit bit = RegisterBit();
	for (int i = 0; i < deviceRegisters[0].GetNumberOfBits(); i++)
	{
		bit.SetDescription("Bit " + std::to_string(i));
		bit.SetIsChecked(true);
		deviceRegisters[0].SetBit(i, bit);
	}

	deviceRegisters.emplace_back("Register 1", 3);
	bit = RegisterBit();
	for (int i = 0; i < deviceRegisters[1].GetNumberOfBits(); i++)
	{
		bit.SetDescription("Bit " + std::to_string(i));
		bit.SetIsChecked(true);
		deviceRegisters[1].SetBit(i, bit);
	}

	deviceRegisters.emplace_back("Register 2", 3);
	bit = RegisterBit();
	for (int i = 0; i < deviceRegisters[2].GetNumberOfBits(); i++)
	{
		bit.SetDescription("Bit " + std::to_string(i));
		bit.SetIsChecked(true);
		deviceRegisters[2].SetBit(i, bit);
	}
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

	HWND hWnd = CreateWindowW(
		szWindowClass,								// Window class name
		szTitle,									// Window title
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,	// Window style
		CW_USEDEFAULT,								// X position
		CW_USEDEFAULT,								// Y position
		550,										// Width
		400,										// Height
		nullptr,									// Parent window
		nullptr,									// Menu
		hInstance,									// Instance handle
		nullptr);									// Additional application data

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
		hbrRegisterArea = CreateSolidBrush(RGB(240, 240, 240));
		hbrWhite = CreateSolidBrush(RGB(255, 255, 255));

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
			180,							// X-position
			80,								// Y-position
			200,							// Width
			30,								// Height
			hWnd,							// Parent window
			(HMENU)ID_TEXTBOX1,				// Control ID
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL							// Pointer not needed
		);

		LoadLightbulbPng(hWnd);

		//break;
	}
				  break;

	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		HWND hwndStatic = (HWND)lParam;

		if (hwndStatic == register_0_label || hwndStatic == register_1_label || hwndStatic == register_2_label)
		{
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, RGB(255, 255, 255));

			if (!hBrush)
			{
				hBrush = CreateSolidBrush(RGB(255, 255, 255));
			}

			// Return the brush to paint the background.
			return (INT_PTR)hBrush;
		}

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
		case ID_CHECKBOX_REG_0_0:
			if (HIWORD(wParam) == BN_CLICKED)
				DisplayCheckBoxState(hWnd, ID_CHECKBOX_REG_0_0, "ID_CHECKBOX_REG_0_0");
			break;
		case ID_CHECKBOX_REG_0_1:
			if (HIWORD(wParam) == BN_CLICKED)
				DisplayCheckBoxState(hWnd, ID_CHECKBOX_REG_0_1, "ID_CHECKBOX_REG_0_1");
			break;
		case ID_CHECKBOX_REG_0_2:
			if (HIWORD(wParam) == BN_CLICKED)
				DisplayCheckBoxState(hWnd, ID_CHECKBOX_REG_0_2, "ID_CHECKBOX_REG_0_2");
			break;
		case ID_CHECKBOX_REG_1_0:
			if (HIWORD(wParam) == BN_CLICKED)
				DisplayCheckBoxState(hWnd, ID_CHECKBOX_REG_1_0, "ID_CHECKBOX_REG_1_0");
			break;
		case ID_CHECKBOX_REG_1_1:
			if (HIWORD(wParam) == BN_CLICKED)
				DisplayCheckBoxState(hWnd, ID_CHECKBOX_REG_1_1, "ID_CHECKBOX_REG_1_1");
			break;
		case ID_CHECKBOX_REG_1_2:
			if (HIWORD(wParam) == BN_CLICKED)
				DisplayCheckBoxState(hWnd, ID_CHECKBOX_REG_1_2, "ID_CHECKBOX_REG_1_2");
			break;
		case ID_CHECKBOX_REG_2_0:
			if (HIWORD(wParam) == BN_CLICKED)
				DisplayCheckBoxState(hWnd, ID_CHECKBOX_REG_2_0, "ID_CHECKBOX_REG_2_0");
			break;
		case ID_CHECKBOX_REG_2_1:
			if (HIWORD(wParam) == BN_CLICKED)
				DisplayCheckBoxState(hWnd, ID_CHECKBOX_REG_2_1, "ID_CHECKBOX_REG_2_1");
			break;
		case ID_CHECKBOX_REG_2_2:
			if (HIWORD(wParam) == BN_CLICKED)
				DisplayCheckBoxState(hWnd, ID_CHECKBOX_REG_2_2, "ID_CHECKBOX_REG_2_2");
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

		// Scale the image.
		float aspectRatio = static_cast<float>(pBitmap->GetWidth()) / pBitmap->GetHeight();
		int newWidth = 100;
		int newHeight = static_cast<int>(newWidth / aspectRatio);
		Gdiplus::Rect destRect(400, 10, newWidth, newHeight);

		// Draw the image.
		graphics.DrawImage(pBitmap, destRect);

		// Cleanup
		delete pBitmap;

		// Draw border

		LONG left = 10L;
		LONG top = 130L;
		LONG right = LONG(left + 510);
		LONG bottom = LONG(top + 195);
		RECT rect = { left, top, right, bottom };

		FillRect(hdc, &rect, hbrRegisterArea);

		// =============================================================

		left = 20L;
		top = 130L + 10L;
		right = (LONG)(left + 150);
		bottom = (LONG)(top + 120);

		int controls_0[] = { ID_CHECKBOX_REG_0_0, ID_CHECKBOX_REG_0_1, ID_CHECKBOX_REG_0_2 };

		register_0_label = PlaceRegisterArea(hWnd, hdc, deviceRegisters[0], controls_0, left, top, right, bottom);

		// =============================================================

		left = right + 20;
		right = (LONG)(left + 150);

		int controls_1[] = { ID_CHECKBOX_REG_1_0, ID_CHECKBOX_REG_1_1, ID_CHECKBOX_REG_1_2 };

		register_1_label = PlaceRegisterArea(hWnd, hdc, deviceRegisters[1], controls_1, left, top, right, bottom);

		// =============================================================

		left = right + 20;
		right = (LONG)(left + 150);

		int controls_2[] = { ID_CHECKBOX_REG_2_0, ID_CHECKBOX_REG_2_1, ID_CHECKBOX_REG_2_2 };

		register_2_label = PlaceRegisterArea(hWnd, hdc, deviceRegisters[2], controls_2, left, top, right, bottom);

		// =============================================================

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

#include <codecvt>

void DisplayCheckBoxState(HWND hWnd, int controlId, std::string controlName)
{
	{
		bool isChecked = IsDlgButtonChecked(hWnd, controlId) == BST_CHECKED;

		std::string str1 = "CheckBox ";
		std::string str2 = controlName;
		std::string str3 = isChecked ? " is checked." : " is unchecked.";

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

		std::wstring wideStr1 = converter.from_bytes(str1);
		std::wstring wideStr2 = converter.from_bytes(str2);
		std::wstring wideStr3 = converter.from_bytes(str3);

		std::wstring combined = wideStr1 + wideStr2 + wideStr3;

		MessageBox(hWnd, combined.c_str(), TEXT("Notification"), MB_OK);
	}
}

HWND PlaceRegisterArea(HWND& hWnd, HDC hdc, DeviceRegister reg, int controls[], LONG& left, LONG& top, LONG& right, LONG& bottom)
{
	// Create a black pen
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

	RECT rect = { left, top, right, bottom };

	// Draw the rectangular border and fill it with white
	Rectangle(hdc, left - 1, top - 1, right + 1, bottom + 1);
	FillRect(hdc, &rect, hbrWhite);

	// Create the label
	HWND register_label = CreateWindow(
		TEXT("STATIC"),					// Predefined class for a label
		reg.GetName_LPCWSTR(),			// Text to display
		WS_CHILD | WS_VISIBLE,			// Styles
		left + 10, top + 10, 100, 20,	// Position and size
		hWnd,							// Parent window
		NULL,							// No menu
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL							// No additional parameters
	);

	for (int bitPosition = 0; bitPosition < reg.GetNumberOfBits(); bitPosition++)
	{
		LPCWSTR description = reg.GetBit(bitPosition).GetDescription_LPCWSTR();
		bool isChecked = reg.GetBit(bitPosition).GetIsChecked();

		HWND registerBit = CreateWindow(
			TEXT("BUTTON"),					// Predefined class for a label
			description,					// Text to display
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,	// Styles
			left + 20, top + (bitPosition * 30) + 30, 100, 20,	// Position and size
			hWnd,							// Parent window
			(HMENU)(UINT_PTR)(controls[bitPosition]),	//
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL							// No additional parameters
		);

		SendMessage(registerBit, BM_SETCHECK, isChecked ? BST_CHECKED : BST_UNCHECKED, 0);
	}

	// Restore the old pen and clean up.
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);

	return register_label;
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

		if (hMemory == NULL)
		{
			MessageBox(hwndParent, TEXT("Failed loading resource."), TEXT("Error"), MB_OK);
			return;
		}

		DWORD imageSize = SizeofResource(hInst, hResource);
		void* pResourceData = LockResource(hMemory);

		HRESULT coInitializeResult = CoInitialize(nullptr);

		if (coInitializeResult != S_OK)
		{
			MessageBox(hwndParent, TEXT("CoInitialize failed."), TEXT("Error"), MB_OK);
			return;
		}

		HRESULT coCreateInstanceResult = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));

		if (coCreateInstanceResult != S_OK)
		{
			MessageBox(hwndParent, TEXT("Failed to creates an instance of the specified COM object."), TEXT("Error"), MB_OK);
		}

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