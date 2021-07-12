#ifndef VIEW_H
#define VIEW_H

#include "observ.h"
#include "gamemodel.h"
#include "types.h"


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
        for (size_t i = 0; i < cells.size(); i++) {
            spr_cell->Render(static_cast<float>(dwBoarder / 2. + cells[i].x * width_cell), static_cast<float>(dhBoarder / 2. + cells[i].y * width_cell));
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
        spr_cell->Render(static_cast<float>(dwBoarder / 2. + cell.x * width_cell), static_cast<float>(dhBoarder / 2. + cell.y * width_cell));
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
        for (size_t i = 0; i < cells.size(); i++) {
            spr_cell->Render(static_cast<float>(dwBoarder / 2. + cells[i].x * width_cell), static_cast<float>(dhBoarder / 2. + cells[i].y * width_cell));
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
        for (size_t i = 0; i < squars.size(); i++) {
            for (size_t j = 0; j < squars.size(); j++) {
                renderQuad(squars[i][j]->leftTop, squars[i][j]->rightTop, squars[i][j]->rightBottom, squars[i][j]->leftBottom, 0xFFFFFFFF);
                //fnt->printf(squars[i][j]->leftTop.x, squars[i][j]->leftTop.y, HGETEXT_LEFT, " %i", squars[i][j]->n);               // TODO перенести во вьюшку
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
        for (auto player : game->players_pawns) {
            const float width_cell = game->board->squars.front()->rightTop.x - game->board->squars.front()->leftTop.x;
            const float dwBoarder = width_cell - player->pawns.front().spr_pawn->GetWidth();
            const float dhBoarder = width_cell - player->pawns.front().spr_pawn->GetHeight();
            for (size_t i = 0; i < player->pawns.size(); i++) {
                player->pawns[i].spr_pawn->Render(static_cast<float>(dwBoarder / 2. + player->pawns[i].x * width_cell), static_cast<float>(dhBoarder / 2. + player->pawns[i].y * width_cell));
            }
        }
        //const pawns_t& pawns = game->players_pawns.front()->pawns;
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
        spr_cursor->SetColor(0xFF00FF00);


        game = model;
        game->addObserver(this);
    }
    ~ViewCursor() {
        delete spr_cursor;
    }
    void renderCell(size_t i, size_t j, size_t border = 2, int color = 0x00FF0000) {
        
        hge->Gfx_RenderLine(game->board->arr_squars[i][j]->leftTop.x + border, game->board->arr_squars[i][j]->leftTop.y+ border, game->board->arr_squars[i][j]->rightTop.x- border, game->board->arr_squars[i][j]->rightTop.y+ border, color);
        hge->Gfx_RenderLine(game->board->arr_squars[i][j]->rightTop.x - border, game->board->arr_squars[i][j]->rightTop.y + border, game->board->arr_squars[i][j]->rightBottom.x - border, game->board->arr_squars[i][j]->rightBottom.y - border, color);
        hge->Gfx_RenderLine(game->board->arr_squars[i][j]->rightBottom.x - border, game->board->arr_squars[i][j]->rightBottom.y - border, game->board->arr_squars[i][j]->leftBottom.x + border, game->board->arr_squars[i][j]->leftBottom.y - border, color);
        hge->Gfx_RenderLine(game->board->arr_squars[i][j]->leftBottom.x + border, game->board->arr_squars[i][j]->leftBottom.y - border, game->board->arr_squars[i][j]->leftTop.x + border, game->board->arr_squars[i][j]->leftTop.y + border, color);

    }

    virtual void update() {
        if (game->stateMouse.choice) {
            //fnt->printf(10, 70, HGETEXT_LEFT, "x=%d y=%d", game->stateMouse.x, game->stateMouse.y);
            renderCell(game->stateMouse.y, game->stateMouse.x, 2, 0xFFFF00FF);
        }
        spr_cursor->Render(game->stateMouse.mx, game->stateMouse.my);
    }
    GameModel* game;
};

class ViewText : public Observer {
    hgeFont* fnt;
public:
    ViewText(GameModel* model) {
        fnt = new hgeFont("G:\\projects\\work\\voroneg\\kvakvs-hge-4f237b4\\tutorials\\precompiled\\font2.fnt");
        game = model;
        game->addObserver(this);

    }
    virtual void update() {
        fnt->printf(game->message.x, game->message.y, HGETEXT_LEFT, "time passed:%f \n%s ", hge->Timer_GetTime(), game->message.str.c_str());
    }
    ~ViewText() {
        delete fnt;
    }
    GameModel* game;
};

class ViewPlaceEnemy : public Observer {
public:
    ViewPlaceEnemy(GameModel* model) {
        game = model;
        game->addObserver(this);
        spr_cell = new hgeSprite(0, 100, 100, 35, 35);
        spr_cell->SetColor(0x33000077);
    }
    ~ViewPlaceEnemy() { delete spr_cell; }

    virtual void update() {
        for (auto player : game->players_pawns) {
            const cells_t& cells = player->enemyPlace->cells;
            const float width_cell = game->board->squars.front()->rightTop.x - game->board->squars.front()->leftTop.x;
            const float dwBoarder = width_cell - spr_cell->GetWidth();
            const float dhBoarder = width_cell - spr_cell->GetHeight();
            for (size_t i = 0; i < cells.size(); i++) {
                spr_cell->Render(static_cast<float>(dwBoarder / 2. + cells[i].x * width_cell), static_cast<float>(dhBoarder / 2. + cells[i].y * width_cell));
            }
        }

    }
    hgeSprite* spr_cell;
    GameModel* game;
};


#endif // VIEW_H

