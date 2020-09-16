#include "framework.h"
#include "Lab_1.h"
#include "Sprite.h"

#define MAX_LOADSTRING 100
#define STEP 3
#define RECTANGLE_HEIGHT 100
#define RECTANGLE_WIDTH 150
#define IMAGE_PATH L"cute_krevetka.bmp"

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
Sprite* sprite;
WCHAR* imagePath = (WCHAR*)IMAGE_PATH;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void				moveObject(RECT, BOOL);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
	// Загрузка таблицы "горячих клавиш"
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

    MSG msg;

    // Цикл основного сообщения:
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);	
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT windowRect;
	static int delta = 0;
	static HBITMAP hBitMap;
	static BITMAP bm;
	static HDC secondDC;
	RECT* tempRect;
	     
    switch (message)
    {
		case WM_CREATE:
			GetClientRect(hWnd, &windowRect);
			sprite = new Sprite{ (windowRect.right - windowRect.left - RECTANGLE_WIDTH) / 2 ,
								 (windowRect.bottom - windowRect.top - RECTANGLE_HEIGHT) / 2, 
								 RECTANGLE_WIDTH, RECTANGLE_HEIGHT, NULL };
			break;
		case WM_SIZE:
			GetClientRect(hWnd, &windowRect);
			sprite->checkBounds(windowRect);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case ID_ACTION_RECT:
				tempRect = sprite->getSpriteRect();

				sprite->setImage(NULL);	
				sprite->setSpriteRect(tempRect->left, tempRect->top, 
									  tempRect->left + RECTANGLE_WIDTH, tempRect->top + RECTANGLE_HEIGHT);

				GetClientRect(hWnd, &windowRect);
				sprite->checkBounds(windowRect);

				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case ID_ACTION_IMAGE:
				sprite->setImage(imagePath);
				HDC hdc;

				hBitMap = (HBITMAP)LoadImage(NULL, sprite->getImage(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				if (hBitMap == NULL)
				{
					MessageBox(hWnd, (WCHAR*)(L"File not found"), (WCHAR*)(L"Error"), MB_OK | MB_ICONHAND);
					sprite->setImage(NULL);
				}
				else
				{
					GetObject(hBitMap, sizeof(bm), &bm);
					hdc = GetDC(hWnd);
					secondDC = CreateCompatibleDC(hdc);
					SelectObject(secondDC, hBitMap);
					ReleaseDC(hWnd, hdc);

					tempRect = sprite->getSpriteRect();
					sprite->setSpriteRect(tempRect->left, tempRect->top, 
										  tempRect->left + bm.bmWidth, tempRect->top + bm.bmHeight);

					GetClientRect(hWnd, &windowRect);
					sprite->checkBounds(windowRect);

					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;
			case ID_ACTION_ANIMATION:
				SetTimer(hWnd, 1, 10, 0);
				break;
			case ID_ACTION_E:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		break;
		case WM_KEYDOWN:
			switch (wParam) {
				case VK_LEFT:
					sprite->move(hWnd, -STEP, 0);
					break;
				case VK_UP:
					sprite->move(hWnd, 0, -STEP);
					break;
				case VK_RIGHT:
					sprite->move(hWnd, STEP, 0);
					break;
				case VK_DOWN:
					sprite->move(hWnd, 0, STEP);
					break;
				case VK_SPACE:
					SetTimer(hWnd, 1, 10, 0);
					break;
				case VK_ESCAPE:
					KillTimer(hWnd, 1);
					break;
			}
			break;
		case WM_MOUSEMOVE:
			if (((MK_LBUTTON == wParam) || (MK_RBUTTON == wParam)) && (sprite->containsCursor(LOWORD(lParam), HIWORD(lParam))))
			{
				sprite->moveTo(hWnd, LOWORD(lParam), HIWORD(lParam));
				/*tempRect = sprite->getSpriteRect();
				if (LOWORD(lParam) > (tempRect->right + tempRect->left) / 2)
				{
					if (HIWORD(lParam) > (tempRect->bottom + tempRect->top) / 2)
					{
						sprite->move(hWnd, STEP, STEP);
					}
					else if (HIWORD(lParam) < (tempRect->bottom + tempRect->top) / 2)
					{
						sprite->move(hWnd, STEP, -STEP);
					} else
					{
						sprite->move(hWnd, STEP, 0);
					}
				}
				else if (LOWORD(lParam) < (tempRect->right + tempRect->left) / 2)
				{
					if (HIWORD(lParam) > (tempRect->bottom + tempRect->top) / 2)
					{
						sprite->move(hWnd, -STEP, STEP);
					}
					else if (HIWORD(lParam) < (tempRect->bottom + tempRect->top) / 2)
					{
						sprite->move(hWnd, -STEP, -STEP);
					} else
					{
						sprite->move(hWnd, -STEP, 0);
					}
				}*/
			}
			break;
		case WM_MOUSEWHEEL:
			delta += GET_WHEEL_DELTA_WPARAM(wParam);
			for (; delta > WHEEL_DELTA; delta -= WHEEL_DELTA)
			{
				if (wParam & MK_SHIFT) {
					sprite->move(hWnd, STEP, 0);
				}
				else {
					sprite->move(hWnd, 0, -STEP);
				}
			}
			for (; delta < 0; delta += WHEEL_DELTA)
			{
				if (wParam & MK_SHIFT) {
					sprite->move(hWnd, -STEP, 0);
				}
				else {
					sprite->move(hWnd, 0, STEP);
				}
			}
			break;
		case WM_ERASEBKGND:
			return (LRESULT)1;
		case WM_PAINT:
			if (sprite != NULL) 
			{
				if (sprite->getImage()) 
				{
					sprite->draw(hWnd, &bm, secondDC);
				}
				else
				{
					sprite->draw(hWnd);
				}
			}
			break;
		case WM_TIMER:
			sprite->moveWithRebound(hWnd, STEP);
			break;
		case WM_DESTROY:
			delete sprite;
			PostQuitMessage(0);
			break;
		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
    return 0;
}