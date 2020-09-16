#include "Sprite.h"
#include <gdiplus.h>

#pragma comment(lib, "Msimg32.lib")

Sprite::Sprite( int left, int top, int width, int height, WCHAR* imagePath ) 
{
	this->spriteRect.left = left;
	this->spriteRect.top = top;
	this->spriteRect.right = left + width;
	this->spriteRect.bottom = top + height;
	this->image = imagePath;
}

RECT* Sprite::getSpriteRect()
{
	return &spriteRect;
}

WCHAR* Sprite::getImage()
{
	return image;
}

void Sprite::setSpriteRect(int left, int top, int right, int bottom)
{
	spriteRect.left = left;
	spriteRect.top = top;
	spriteRect.right = right;
	spriteRect.bottom = bottom;
}

void Sprite::setImage(WCHAR* image)
{
	this->image = image;
}

BOOL Sprite::containsCursor(int cursorPosX, int cursorPosY) 
{
	return cursorPosX >= spriteRect.left
		&& cursorPosX <= spriteRect.right
		&& cursorPosY >= spriteRect.top
		&& cursorPosY <= spriteRect.bottom;
}

void Sprite::move(HWND hWnd, int dx, int dy, BOOL rebound)
{
	InvalidateRect(hWnd, &spriteRect, TRUE);

	RECT windowRect;
	GetClientRect(hWnd, &windowRect);

	spriteRect.left += dx;
	spriteRect.top += dy;
	spriteRect.right += dx;
	spriteRect.bottom += dy;

	if (rebound)
	{
		this->rebound(windowRect);
	}
	else
	{
		checkBounds(windowRect);
	}

	InvalidateRect(hWnd, &spriteRect, FALSE);
}

void Sprite::moveTo(HWND hWnd, int cursorPosX, int cursorPosY) 
{
	int oldLeft = spriteRect.left,
		oldTop = spriteRect.top,
		oldRight = spriteRect.right,
		oldBottom = spriteRect.bottom;
	RECT windowRect;

	InvalidateRect(hWnd, &spriteRect, TRUE);
	
	GetClientRect(hWnd, &windowRect);

	spriteRect.left = cursorPosX - (oldRight - oldLeft) / 2;
	spriteRect.top = cursorPosY - (oldBottom - oldTop) / 2;
	spriteRect.right = cursorPosX + (oldRight - oldLeft) / 2;
	spriteRect.bottom = cursorPosY + (oldBottom - oldTop) / 2;

	checkBounds(windowRect);

	InvalidateRect(hWnd, &spriteRect, FALSE);
}

void Sprite::moveWithRebound(HWND hWnd, int step)
{
	if ((movementDirectionX == TRUE) && (movementDirectionY == TRUE)) {
		move(hWnd, step, step, TRUE);
	}
	else if ((movementDirectionX == FALSE) && (movementDirectionY == TRUE)) {
		move(hWnd, -step, step, TRUE);
	}
	else if ((movementDirectionX == FALSE) && (movementDirectionY == FALSE)) {
		move(hWnd, -step, -step, TRUE);
	}
	else if ((movementDirectionX == TRUE) && (movementDirectionY == FALSE)) {
		move(hWnd, step, -step, TRUE);
	}
}

void Sprite::rebound(RECT windowRect)
{
	if (spriteRect.left < windowRect.left)
	{
		spriteRect.right -= spriteRect.left;
		spriteRect.left = windowRect.left;
		movementDirectionX = !movementDirectionX;
	}
	if (spriteRect.top < windowRect.top)
	{
		spriteRect.bottom -= spriteRect.top;
		spriteRect.top = windowRect.top;
		movementDirectionY = !movementDirectionY;
	}
	if (spriteRect.right > windowRect.right)
	{
		spriteRect.left -= (spriteRect.right - windowRect.right);
		spriteRect.right = windowRect.right;
		movementDirectionX = !movementDirectionX;
	}
	if (spriteRect.bottom > windowRect.bottom)
	{
		spriteRect.top -= (spriteRect.bottom - windowRect.bottom);
		spriteRect.bottom = windowRect.bottom;
		movementDirectionY = !movementDirectionY;
	}
}

void Sprite::checkBounds(RECT windowRect) 
{
	if (spriteRect.left < windowRect.left)
	{
		spriteRect.right -= spriteRect.left;
		spriteRect.left = windowRect.left;
	}
	if (spriteRect.top < windowRect.top)
	{
		spriteRect.bottom -= spriteRect.top;
		spriteRect.top = windowRect.top;
	}
	if (spriteRect.right > windowRect.right)
	{
		spriteRect.left -= ( spriteRect.right - windowRect.right);
		spriteRect.right = windowRect.right;
	}
	if (spriteRect.bottom > windowRect.bottom)
	{
		spriteRect.top -= ( spriteRect.bottom - windowRect.bottom);
		spriteRect.bottom = windowRect.bottom;
	}
}

void Sprite::draw(HWND hWnd, BITMAP* bm, HDC memBit)
{
	HDC hdc, hdcMem;
	PAINTSTRUCT ps;
	HBITMAP hbmMem, hbmOld;
	HBRUSH hBrush, hBkGroundBrush;
	RECT windowRect;

	hdc = BeginPaint(hWnd, &ps);

	GetClientRect(hWnd, &windowRect);

	hdcMem = CreateCompatibleDC(hdc);
	hbmMem = CreateCompatibleBitmap(hdc, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

	hBkGroundBrush = CreateSolidBrush(RGB(20, 30, 40));
	FillRect(hdcMem, &windowRect, hBkGroundBrush);
	DeleteObject(hBkGroundBrush);

	SetBkMode(hdcMem, TRANSPARENT);

	if (this->image) {
		TransparentBlt(hdcMem, spriteRect.left, spriteRect.top, bm->bmWidth, bm->bmHeight,
			memBit, 0, 0, bm->bmWidth, bm->bmHeight, RGB(0, 0, 0));
		//BitBlt(hdcMem, spriteRect.left, spriteRect.top, bm->bmWidth, bm->bmHeight, memBit, 0, 0, SRCCOPY);
	}
	else {
		hBrush = CreateSolidBrush(RGB(30, 180, 200));
		SelectObject(hdcMem, hBrush);
		Rectangle(hdcMem, spriteRect.left, spriteRect.top, spriteRect.right, spriteRect.bottom);
	}

	BitBlt(hdc, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, hbmOld);
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);

	EndPaint(hWnd, &ps);
}
