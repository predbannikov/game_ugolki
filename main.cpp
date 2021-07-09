#include "main.h"
#include <stdio.h>
#include <iostream>


HGE* hge = nullptr;

GameModel* game;
ViewGame* view;
ViewCursor* cursor;
Controller* controller;
ViewText* text;
ViewBoard* viewBoard;
ViewPawns* viewPawns;
hgeFont* fnt;

Game* doska;


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
    hgeColorRGB hgeCol(0.0, 0.0, 0.0, 1.0);
    hge->Gfx_Clear(hgeCol.GetHWColor());
    //doska->update();

    controller->perform();


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
    fnt = new hgeFont("G:\\projects\\work\\voroneg\\kvakvs-hge-4f237b4\\tutorials\\precompiled\\font2.fnt");
    game = new GameModel;
    view = new ViewGame(game);
    text = new ViewText(game);
    viewBoard = new ViewBoard(game);
    viewPawns = new ViewPawns(game);
    cursor = new ViewCursor(game);      // курсор добавлять последним чтоб отрисовывался поверх 
    controller = new Controller(game);
    //doska = new Game(50);
}

void done_window() {
    //delete spr_cursor;
    delete fnt;
}

