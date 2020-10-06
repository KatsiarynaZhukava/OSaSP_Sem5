#include "framework.h"
#include "Lab_2.h"
#include <string>
#include <vector>
#include <fstream>
#include <Commdlg.h>
#include <cmath>

struct CellNumber {
	int rowNumber;
	int columnNumber;
};

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif


#define MAX_LOADSTRING 100
#define MIN_WINDOW_WIDTH 450
#define MIN_WNDOW_HEIGHT 500
#define DEFAULT_COLUMNS_NUMBER 6

HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
WCHAR szChildWindowClass[MAX_LOADSTRING];		// имя класса дочернего окна
WCHAR szChildTitle[MAX_LOADSTRING];             // Текст строки заголовка дочернего окна

HFONT windowFont;

RECT windowRect;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int					DrawTable(HWND hWnd, RECT windowRect, std::vector<std::string> tableCellsText);

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

    if (!InitInstance (hInstance, nCmdShow))
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

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = CreateSolidBrush(RGB(221, 217, 240));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   HWND hWnd = CreateWindowW(szWindowClass, (LPCWSTR)"Adjusting table", WS_OVERLAPPEDWINDOW,
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
	static OPENFILENAME openfn;
	static std::ifstream input;
	static OPENFILENAME openFileName;
	static TCHAR szFile[260] = { 0 };

    switch (message)
    {
	case WM_CREATE:	

		ZeroMemory(&openFileName, sizeof(openFileName));
		openFileName.lStructSize = sizeof(openFileName);
		openFileName.hwndOwner = hWnd;
		openFileName.lpstrFile = szFile;
		openFileName.nMaxFile = sizeof(szFile);
		openFileName.lpstrFilter = "Text files only(*.txt)\0*.txt";
		openFileName.nFilterIndex = 1;
		openFileName.lpstrFileTitle = NULL;
		openFileName.nMaxFileTitle = 0;
		openFileName.lpstrInitialDir = NULL;
		openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		windowFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, 
								CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_MODERN | FF_DONTCARE, _T("Comic Sans MS"));
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
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case IDM_CLEAR_TABLE:
			tableCellsText.clear();
			InvalidateRect(hWnd, NULL, TRUE);
			break;
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
    case WM_PAINT:
		GetClientRect(hWnd, &windowRect);
		DrawTable(hWnd, windowRect, tableCellsText);
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


int DrawTable(HWND hWnd, RECT windowRect, std::vector<std::string> tableCellsText)
{
	HDC hdc, hdcMem;
	PAINTSTRUCT ps;
	HBITMAP hbmMem, hbmOld;
	COLORREF borderColor = RGB(132, 139, 163),
			 textColor = RGB(46, 49, 64);
	HPEN hPen = CreatePen(PS_SOLID, 1, borderColor);
	HBRUSH hBackgroundBrush = CreateSolidBrush(RGB(228, 221, 246));
	RECT cellRectangle;
	int indent = 3,
		defaultCellHeight = 30,
		currentHeight = 0,
		columnsNumber = 0;

	hdc = BeginPaint(hWnd, &ps);


	hdcMem = CreateCompatibleDC(hdc);
	hbmMem = CreateCompatibleBitmap(hdc, (windowRect.right - windowRect.left), (windowRect.bottom - windowRect.top));
	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

	SelectObject(hdcMem, hBackgroundBrush);
	Rectangle(hdcMem, windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);


	SelectObject(hdcMem, hPen);
	if (tableCellsText.size() > 0)
	{
		columnsNumber = round(sqrt(tableCellsText.size()));
		if (columnsNumber > DEFAULT_COLUMNS_NUMBER)
		{
			columnsNumber = DEFAULT_COLUMNS_NUMBER;
		}

		for (int i = 0; i <= columnsNumber; i++)
		{
			MoveToEx(hdcMem, i * windowRect.right / columnsNumber, 0, NULL);
			LineTo(hdcMem, i * windowRect.right / columnsNumber, windowRect.bottom);
		}		
	}
	else
	{
		columnsNumber = DEFAULT_COLUMNS_NUMBER;
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

				DrawText(hdcMem, tableCellsText.at(i * columnsNumber + j).c_str(), -1, &cellRectangle, DT_CALCRECT | DT_WORDBREAK );

				if (currentHeight < cellRectangle.bottom)
				{
					currentHeight = cellRectangle.bottom;
				}
				DrawText(hdcMem, tableCellsText.at(i * columnsNumber + j).c_str(), -1, &cellRectangle, DT_WORDBREAK );
			}

		}

		MoveToEx(hdcMem, 0, currentHeight, NULL);
		LineTo(hdcMem, windowRect.right, currentHeight);

		i++;
	}

	BitBlt(hdc, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOld);

	EndPaint(hWnd, &ps);

	DeleteObject(hbmMem);
	DeleteDC(hdcMem);
	DeleteObject(hPen);
	DeleteObject(hBackgroundBrush);

	return currentHeight;
}