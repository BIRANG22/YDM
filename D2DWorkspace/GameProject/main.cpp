#include "Game.h"
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Game game;
    if (!game.Initialize(hInstance, nCmdShow))
        return -1;

    game.Run();

    return 0;
}
