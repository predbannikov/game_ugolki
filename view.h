#ifndef VIEW_H
#define VIEW_H

#include "observ.h"
#include "gamemodel.h"
#include "game.h"

struct ViewPlaceDebug : public Observer {
    ViewPlaceDebug(GameModel* model) {
        game = model;
        game->addObserver(this);
        spr_cell = new hgeSprite(0, 100, 100, 35, 35);
        spr_cell->SetColor(0x99666666);
    }

    virtual void update() {
        const cells_t& cells = game->cells_debug;
        const float width_cell = game->board->squars.front()->rightTop.x - game->board->squars.front()->leftTop.x;
        const float dwBoarder = width_cell - spr_cell->GetWidth();
        const float dhBoarder = width_cell - spr_cell->GetHeight();
        for (int i = 0; i < cells.size(); i++) {
            spr_cell->Render(dwBoarder / 2. + cells[i].x * width_cell, dhBoarder / 2. + cells[i].y * width_cell);
        }

    }
    hgeSprite* spr_cell;
    GameModel* game;
};


struct ViewPointDebug : public Observer {
    ViewPointDebug(GameModel* model) {
        game = model;
        game->addObserver(this);
        spr_cell = new hgeSprite(0, 100, 100, 35, 35);
        spr_cell->SetColor(0x99008800);
    }

    virtual void update() {
        const Cell& cell = game->cell_debug;
        const float width_cell = game->board->squars.front()->rightTop.x - game->board->squars.front()->leftTop.x;
        const float dwBoarder = width_cell - spr_cell->GetWidth();
        const float dhBoarder = width_cell - spr_cell->GetHeight();
        spr_cell->Render(dwBoarder / 2. + cell.x * width_cell, dhBoarder / 2. + cell.y * width_cell);
    }
    hgeSprite* spr_cell;
    GameModel* game;
};

struct ViewCellsTarget : public Observer {
    ViewCellsTarget(GameModel* model) {
        game = model;
        game->addObserver(this);
        spr_cell = new hgeSprite(0, 100, 100, 35, 35);
        spr_cell->SetColor(0x55880000);
    }

    virtual void update() {
        const cells_t& cells = game->cells_target;
        const float width_cell = game->board->squars.front()->rightTop.x - game->board->squars.front()->leftTop.x;
        const float dwBoarder = width_cell - spr_cell->GetWidth();
        const float dhBoarder = width_cell - spr_cell->GetHeight();
        for (int i = 0; i < cells.size(); i++) {
            spr_cell->Render(dwBoarder / 2. + cells[i].x * width_cell, dhBoarder / 2. + cells[i].y * width_cell);
        }

    }
    hgeSprite* spr_cell;
    GameModel* game;
};

struct ViewBoard : public Observer {
    ViewBoard(GameModel* model) {
        game = model;
        game->addObserver(this);
    }
    void renderQuad(const PointF& p1, const PointF& p2, const PointF& p3, const PointF& p4, int color = 0x00FF0000) {
        hge->Gfx_RenderLine(p1.x, p1.y, p2.x, p2.y, color);
        hge->Gfx_RenderLine(p2.x, p2.y, p3.x, p3.y, color);
        hge->Gfx_RenderLine(p3.x, p3.y, p4.x, p4.y, color);
        hge->Gfx_RenderLine(p4.x, p4.y, p1.x, p1.y, color);
    }

    virtual void update() {
        const arr_squars_t& squars = game->board->arr_squars;
        for (int i = 0; i < squars.size(); i++) {
            for (int j = 0; j < squars.size(); j++) {
                renderQuad(squars[i][j]->leftTop, squars[i][j]->rightTop, squars[i][j]->rightBottom, squars[i][j]->leftBottom, 0xFFFFFFFF);
                fnt->printf(squars[i][j]->leftTop.x, squars[i][j]->leftTop.y, HGETEXT_LEFT, " %i", squars[i][j]->n);               // TODO перенести во вьюшку
            }
        }
    }
    GameModel* game;
};

struct ViewPawns : public Observer {
    ViewPawns(GameModel* model) {
        game = model;
        game->addObserver(this);
    }


    virtual void update() {
        const pawns_t& pawns = game->players_pawns.front()->pawns;
        const float width_cell = game->board->squars.front()->rightTop.x - game->board->squars.front()->leftTop.x;
        const float dwBoarder = width_cell - pawns.front().spr_pawn->GetWidth();
        const float dhBoarder = width_cell - pawns.front().spr_pawn->GetHeight();
        for (int i = 0; i < pawns.size(); i++) {
            pawns[i].spr_pawn->Render(dwBoarder / 2. + pawns[i].x * width_cell, dhBoarder / 2. + pawns[i].y * width_cell);
        }
    }
    GameModel* game;
};

struct ViewGame : public Observer {
    ViewGame(GameModel* model) {
        game = model;
        game->addObserver(this);
    }
    virtual void update() {

    }
    GameModel* game;
};

class ViewCursor : public Observer {
    hgeSprite* spr_cursor;

public:
    ViewCursor(GameModel* model) {

        spr_cursor = new hgeSprite(0, 487, 181, 16, 16);
        hgeColorRGB cur_color(0, 1, 0, 1);
        spr_cursor->SetColor(cur_color.GetHWColor());


        game = model;
        game->addObserver(this);
    }
    ~ViewCursor() {
        delete spr_cursor;
    }
    virtual void update() {
        spr_cursor->Render(game->stateMouse.mx, game->stateMouse.my);
    }
    GameModel* game;
};

class ViewText : public Observer {
public:
    ViewText(GameModel* model) {
        game = model;
        game->addObserver(this);
    }
    virtual void update() {
        fnt->printf(game->message.x, game->message.y, HGETEXT_LEFT, "Message: %s %f", game->message.str.c_str(), hge->Timer_GetTime());
    }
    GameModel* game;
};

class ViewPlaceEnemy : public Observer {
public:
    ViewPlaceEnemy(GameModel* model) {
        game = model;
        game->addObserver(this);
        spr_cell = new hgeSprite(0, 100, 100, 35, 35);
        spr_cell->SetColor(0x55000077);
    }

    virtual void update() {
        for (auto player : game->players_pawns) {
            const cells_t& cells = player->enemyPlace->cells;
            const float width_cell = game->board->squars.front()->rightTop.x - game->board->squars.front()->leftTop.x;
            const float dwBoarder = width_cell - spr_cell->GetWidth();
            const float dhBoarder = width_cell - spr_cell->GetHeight();
            for (int i = 0; i < cells.size(); i++) {
                spr_cell->Render(dwBoarder / 2. + cells[i].x * width_cell, dhBoarder / 2. + cells[i].y * width_cell);
            }
        }

    }
    hgeSprite* spr_cell;
    GameModel* game;
};


#endif // VIEW_H

