#include <windows.h>
#include <stdio.h>

#include "graphics.h"
#include <iostream>
#include <list>

using std::list;

int t = 10;
int dirX = +1;
int dirY = 0;


struct Point
{
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
};

list<Point> snake;
Point food(rand() % 60, rand() % 40);

DWORD WINAPI render(LPVOID arg)
{
    Graphics *g = (Graphics *)arg;
    while (true)
    {
        g->clearBuffer();

        for (auto segment : snake)
        {
            g->fillPixel(segment.x, segment.y, BACKGROUND_GREEN);
        }

        g->fillPixel(food.x, food.y, BACKGROUND_RED);

        int x = snake.front().x + dirX;
        int y = snake.front().y + dirY;

        if (snake.front().x == food.x && snake.front().y == food.y) {
            food.x = rand() % 60;
            food.y = rand() % 40;
        } else {
            snake.pop_back();
        }
        snake.push_front(Point(x, y));

        g->render();
        Sleep(200);
    }
}

int main(void)
{
    HANDLE hStdin, hStdout, hNewScreenBuffer;
    BOOL fSuccess;

    INPUT_RECORD irec;
    DWORD cc;

    // Get a handle to the STDOUT screen buffer to copy from and
    // create a new screen buffer to copy to.

    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    hNewScreenBuffer = CreateConsoleScreenBuffer(
        GENERIC_READ | // read/write access
            GENERIC_WRITE,
        FILE_SHARE_READ |
            FILE_SHARE_WRITE,    // shared
        NULL,                    // default security attributes
        CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE
        NULL);                   // reserved; must be NULL
    if (hStdout == INVALID_HANDLE_VALUE ||
        hNewScreenBuffer == INVALID_HANDLE_VALUE)
    {
        printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
        return 1;
    }

    // Make the new screen buffer the active screen buffer.

    if (!SetConsoleActiveScreenBuffer(hNewScreenBuffer))
    {
        printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
        return 1;
    }

    snake.push_back(Point(1, 1));
    snake.push_back(Point(2, 1));
    snake.push_back(Point(3, 1));

    Graphics g(hNewScreenBuffer, 40, 60);

    bool running = true;

    DWORD threadId;
    HANDLE thread = CreateThread(NULL, 0, render, &g, 0, &threadId);

    while (running)
    {
        ReadConsoleInput(hStdin, &irec, 1, &cc);
        if (irec.EventType == KEY_EVENT && ((KEY_EVENT_RECORD &)irec.Event).bKeyDown)
        {
            KEY_EVENT_RECORD krec = (KEY_EVENT_RECORD &)irec.Event;

            switch (krec.wVirtualKeyCode)
            {
            case VK_DOWN:
                dirX = 0;
                dirY = +1;
                break;
            case VK_UP:
                dirX = 0;
                dirY = -1;
                break;
            case VK_LEFT:
                dirX = -1;
                dirY = 0;
                break;
            case VK_RIGHT:
                dirX = 1;
                dirY = 0;
                break;
            case VK_ESCAPE:
                running = false;
                break;
            }
        }
    }

    // Restore the original active screen buffer.

    if (!SetConsoleActiveScreenBuffer(hStdout))
    {
        printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
        return 1;
    }

    return 0;
}