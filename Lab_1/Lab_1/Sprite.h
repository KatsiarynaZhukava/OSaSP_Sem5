#pragma once
#include <windows.h>


class Sprite
{
	private:
		RECT spriteRect;
		WCHAR* image;
		BOOL movementDirectionX = TRUE, movementDirectionY = TRUE;

	public:
		Sprite( int left, int top, int width, int height, WCHAR* image );

		RECT* getSpriteRect();

		WCHAR* getImage();

		void setSpriteRect(int left, int top, int right, int bottom);

		void setImage(WCHAR* image);

		BOOL containsCursor(int cursorPosX, int cursorPosY);

		void move(HWND hWnd, int x, int y, BOOL rebound = FALSE);

		void moveTo(HWND hWnd, int x, int y);

		void moveWithRebound(HWND hWnd, int step);

		void rebound(RECT windowRect);

		void checkBounds(RECT windowRect);

		void draw(HWND hwnd, BITMAP* bm = NULL, HDC memBit = NULL);
};

