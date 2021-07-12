/*
* Тут ничего интересного, создали Game, указали где будем обновлять кадры game->update()
*/
#include <stdio.h>
#include <iostream>
#include "game.h"

HGE* hge = nullptr;
Game* game = nullptr;



void init_window();
void done_window();


bool handle_keys(const int key) {
    switch (key) {
    case 0:
        return false;
    case HGEK_ESCAPE:
        return true;
    }
    return false;
}

bool frame_func() {

    if (handle_keys(hge->Input_GetKey())) {
        return true;
    }
    return false;
}

bool render_func() {


    hge->Gfx_BeginScene();
    hge->Gfx_Clear(0x00000000);

    game->updater();


    hge->Gfx_EndScene();
    return false;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {


    hge = hgeCreate(HGE_VERSION);
    hge->System_SetState(HGE_FRAMEFUNC, frame_func);
    hge->System_SetState(HGE_RENDERFUNC, render_func);
    hge->System_SetState(HGE_TITLE, "HGE");
    hge->System_SetState(HGE_SCREENWIDTH, 800);
    hge->System_SetState(HGE_SCREENHEIGHT, 600);
    hge->System_SetState(HGE_SCREENBPP, 32);
    hge->System_SetState(HGE_USESOUND, false);

    if (hge->Ini_GetInt("HGE", "FullScreen", 0)) {
        hge->System_SetState(HGE_WINDOWED, false);
    }
    else {
        hge->System_SetState(HGE_WINDOWED, true);
    }

    if (hge->System_Initiate()) {

        init_window();
        hge->System_Start();
        done_window();
    }
    else {
        MessageBox(nullptr, hge->System_GetErrorMessage(), "Error",
            MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }

    hge->System_Shutdown();
    hge->Release();
    return 0;
}

void init_window() {
    game = new Game;
}

void done_window() {
    delete game;
}

