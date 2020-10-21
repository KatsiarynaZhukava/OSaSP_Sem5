#include "framework.h"
#include "Lab_2.h"
#include <string>
#include <vector>
#include <fstream>
#include <Commdlg.h>
#include <cmath>


#define MAX_LOADSTRING 100
#define MIN_WINDOW_WIDTH 200
#define MIN_WNDOW_HEIGHT 200
#define DEFAULT_COLUMNS_NUMBER 5
#define SCROLL_STEP 30


int tableTop = 0;
int tableHeight, clAreaWidth, clAreaHeight;
HFONT windowFont;
RECT windowRect;
SCROLLINFO scrollInfo;


HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
WCHAR szChildWindowClass[MAX_LOADSTRING];		// имя класса дочернего окна
WCHAR szChildTitle[MAX_LOADSTRING];             // Текст строки заголовка дочернего окна

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int					DrawTable(HWND hWnd, RECT windowRect, std::vector<std::string> tableCellsText);
void				UpdateScrollInfo(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(221, 217, 240));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB2);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, (LPCWSTR)"Adjusting table", WS_OVERLAPPEDWINDOW | WS_VSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static std::vector<std::string> tableCellsText;
	static std::string str;
	static std::ifstream input;
	static OPENFILENAME openFileName;
	static TCHAR fileNameBuffer[200] = { 0 };
	int currentPos, wheelDelta, step = 0;

	switch (message)
	{
	case WM_CREATE:
		ZeroMemory(&openFileName, sizeof(openFileName));
		openFileName.lStructSize = sizeof(openFileName);
		openFileName.hwndOwner = hWnd;
		openFileName.lpstrFile = fileNameBuffer;
		openFileName.nMaxFile = sizeof(fileNameBuffer);
		openFileName.lpstrFilter = "Text files only(*.txt)\0*.txt";
		openFileName.nFilterIndex = 1;
		openFileName.nMaxFileTitle = 0;

		windowFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_MODERN | FF_DONTCARE, _T("Comic Sans MS"));
		break;
	case WM_SIZE:
		clAreaHeight = HIWORD(lParam);
		tableTop = 0;
		InvalidateRect(hWnd, NULL, FALSE);
		UpdateWindow(hWnd);
		UpdateScrollInfo(hWnd);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_OPEN_FILE:

			if (GetOpenFileName(&openFileName) == TRUE)
			{
				input.open(openFileName.lpstrFile);
				if (input.is_open())
				{
					tableCellsText.clear();

					while (getline(input, str))
					{
						tableCellsText.push_back(str);
					}
					input.close();
				}
				else
				{
					MessageBox(hWnd, "File not found", "Error", MB_OK);
				}
			}
			break;
		case IDM_CLEAR_TABLE:
			tableCellsText.clear();
			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		UpdateWindow(hWnd);
		UpdateScrollInfo(hWnd);
		break;
	case WM_MOUSEWHEEL:
		wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (wheelDelta > 0)
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
		if (wheelDelta < 0)
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			SendMessage(hWnd, WM_VSCROLL, SB_PAGEDOWN, NULL);
			break;
		case VK_RIGHT:
			SendMessage(hWnd, WM_VSCROLL, SB_PAGEUP, NULL);
			break;
		case VK_UP:
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			break;
		case VK_DOWN:
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			break;
		}
		break;
	case WM_VSCROLL:

		scrollInfo.cbSize = sizeof(SCROLLINFO);

		scrollInfo.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &scrollInfo);

		currentPos = scrollInfo.nPos;

		switch (LOWORD(wParam)) {
		case SB_LINEUP:
			scrollInfo.nPos -= SCROLL_STEP;
			step = SCROLL_STEP;
			break;
		case SB_LINEDOWN:
			scrollInfo.nPos += SCROLL_STEP;
			step = -SCROLL_STEP;
			break;
		case SB_PAGEUP:
			scrollInfo.nPos -= scrollInfo.nPage;
			step = scrollInfo.nPage;
			break;
		case SB_PAGEDOWN:
			scrollInfo.nPos += scrollInfo.nPage;
			step = -(int)scrollInfo.nPage;
			break;
		case SB_THUMBTRACK:
			scrollInfo.nPos = scrollInfo.nTrackPos;
			step = currentPos - scrollInfo.nPos;
			break;
		default: return 0;
		}

		scrollInfo.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &scrollInfo, TRUE);

		tableTop += step;

		if (((step > 0) && (tableTop - step < 0)) || ((step < 0) && (tableHeight - clAreaHeight > 0)))
		{
			ScrollWindow(hWnd, 0, step, NULL, NULL);
			UpdateWindow(hWnd);
		}
		else
		{
			tableTop -= step;
		}

		break;
	case WM_ERASEBKGND:
		return (LRESULT)1;
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = MIN_WINDOW_WIDTH;
		lpMMI->ptMinTrackSize.y = MIN_WNDOW_HEIGHT;
	}
	break;
	case WM_PAINT:
		GetClientRect(hWnd, &windowRect);
		tableHeight = DrawTable(hWnd, windowRect, tableCellsText);
		break;
	case WM_DESTROY:
		DeleteObject(windowFont);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void UpdateScrollInfo(HWND hWnd)
{
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	scrollInfo.nPos = 0;
	scrollInfo.nPage = clAreaHeight;
	scrollInfo.nMin = 0;
	scrollInfo.nMax = tableHeight;
	scrollInfo.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
	SetScrollInfo(hWnd, SB_VERT, &scrollInfo, TRUE);
}

void DrawLine(HDC hdc, int x0, int y0, int x1, int y1)
{
	MoveToEx(hdc, x0, y0, NULL);
	LineTo(hdc, x1, y1);
}

int DrawTable(HWND hWnd, RECT windowRect, std::vector<std::string> tableCellsText)
{
	HDC hdc, hdcMem;
	PAINTSTRUCT ps;
	HBITMAP hbmMem, hbmOld;
	COLORREF borderColor = RGB(109, 118, 138),
		textColor = RGB(51, 54, 61),
		backgroundColor = RGB(225, 230, 240);
	HPEN hPen = CreatePen(PS_SOLID, 1, borderColor);
	HBRUSH hBackgroundBrush = CreateSolidBrush(backgroundColor);
	RECT cellRectangle;
	int indent = 3,
		defaultCellHeight = 30,
		currentHeight = tableTop,
		columnsNumber = DEFAULT_COLUMNS_NUMBER;

	hdc = BeginPaint(hWnd, &ps);


	hdcMem = CreateCompatibleDC(hdc);
	hbmMem = CreateCompatibleBitmap(hdc, (windowRect.right - windowRect.left), (windowRect.bottom - windowRect.top));
	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

	SelectObject(hdcMem, hBackgroundBrush);
	FillRect(hdcMem, &windowRect, hBackgroundBrush);

	SelectObject(hdcMem, hPen);

	if (tableCellsText.size() > 0)
	{
		columnsNumber = round(sqrt(tableCellsText.size()));
		//columnsNumber = DEFAULT_COLUMNS_NUMBER;
		if (columnsNumber > DEFAULT_COLUMNS_NUMBER)
		{
			columnsNumber = DEFAULT_COLUMNS_NUMBER;
		}

		for (int i = 0; i < columnsNumber; i++)
		{
			DrawLine(hdcMem, i * windowRect.right / columnsNumber, 0,
				i * windowRect.right / columnsNumber, windowRect.bottom);
		}

		DrawLine(hdcMem, windowRect.right - 1, 0, windowRect.right - 1, windowRect.bottom);
		DrawLine(hdcMem, 0, currentHeight, windowRect.right, currentHeight);
	}

	SelectObject(hdcMem, windowFont);

	int i = 0;
	while (tableCellsText.size() >= i * columnsNumber)
	{
		cellRectangle.top = currentHeight + indent;

		SetBkMode(hdcMem, TRANSPARENT);
		SetTextColor(hdcMem, textColor);

		for (int j = 0; j <= columnsNumber; j++)
		{
			if (tableCellsText.size() > (i * columnsNumber + j))
			{
				cellRectangle.left = j * windowRect.right / columnsNumber + indent;
				cellRectangle.right = (j + 1) * windowRect.right / columnsNumber - indent;

				DrawText(hdcMem, tableCellsText.at(i * columnsNumber + j).c_str(), -1, &cellRectangle, DT_CALCRECT | DT_EDITCONTROL | DT_WORDBREAK);

				if (currentHeight < cellRectangle.bottom)
				{
					currentHeight = cellRectangle.bottom;
				}
				DrawText(hdcMem, tableCellsText.at(i * columnsNumber + j).c_str(), -1, &cellRectangle, DT_EDITCONTROL | DT_WORDBREAK);
			}
		}

		DrawLine(hdcMem, 0, currentHeight, windowRect.right, currentHeight);
		i++;
	}

	BitBlt(hdc, windowRect.left, windowRect.top, windowRect.right, windowRect.bottom - windowRect.top,
		hdcMem, 0, 0, SRCCOPY);


	SelectObject(hdcMem, hbmOld);

	EndPaint(hWnd, &ps);

	DeleteObject(hbmMem);
	DeleteDC(hdcMem);
	DeleteObject(hPen);
	DeleteObject(hBackgroundBrush);

	return currentHeight;
}