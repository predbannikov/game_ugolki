#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "gamemodel.h"
#include "controller.h"
#include "view.h"



class Game {
    GameModel* model;
    ViewGame* view;
    ViewCursor* cursor;
    ViewText* text;
    ViewBoard* viewBoard;
    ViewPawns* viewPawns;
    ViewPlaceEnemy* viewPlaceEnemy;
    ViewPlaceDebug* viewPlaceDebug;
    ViewCellsTarget* viewCellsTarget;
    ViewPointDebug* viewPointDebug;
    Controller* controller;
    Controller* controller2;
    //hgeFont* fnt;
    bool launched = false;

    enum STATE_ORDER_STEP {STATE_STEP_ONE, STATE_STEP_TWO} state_order_state = STATE_STEP_ONE;

public:
    Game() {
        model = new GameModel;
        view = new ViewGame(model);
        viewPawns = new ViewPawns(model);
        viewPlaceEnemy = new ViewPlaceEnemy(model);
        viewPlaceDebug = new ViewPlaceDebug(model);
        viewCellsTarget = new ViewCellsTarget(model);
        viewPointDebug = new ViewPointDebug(model);
        viewBoard = new ViewBoard(model);
        text = new ViewText(model);
        cursor = new ViewCursor(model);      // курсор добавлять последним чтоб отрисовывался поверх 
        // TODO создать контроллер с указанием кто им будет играть
        // 
    }
    void createPlayers(size_t width, size_t heigth, Point p1, size_t count_player_human = 1) {
        controller = new Controller(model, true, p1, width, heigth);
        Cell cell = model->players_pawns.front()->enemyPlace->cells.back();
        Point secondPlayer = Point(cell.x, cell.y);
        controller2 = new Controller(model, false, secondPlayer, width, heigth);
    }


    void updater() {
        model->moveCursore();
        if (!launched && hge->Input_IsMouseOver()) {
            if (model->clickCursor()) {
                if (model->cells_target.empty()) {
                    return;
                }
                Point target_point(model->cells_target.front().x, model->cells_target.front().y);

                int width = model->cells_target.back().x - target_point.x + 1;
                int height = model->cells_target.back().y - target_point.y + 1;
                model->cells_target.clear();
                if (width <= 0 && height <= 0) {
                    return;
                }
                if ((target_point.x < model->board->count_cells_col / 2 && target_point.x + width > model->board->count_cells_col / 2) && (target_point.y < model->board->count_cells_row / 2 && target_point.y + height > model->board->count_cells_row / 2))
                    return;


                createPlayers(width, height, target_point);
                launched = true;
            }

        }
        if (launched) {
            if (state_order_state == STATE_STEP_ONE) {
                if (controller->perform()) {
                    state_order_state = STATE_STEP_TWO;
                }
            } else {
                if (controller2->perform()) {
                    state_order_state = STATE_STEP_ONE;
                }
            }
        }
        else {
            model->printString("choose number ");
        }
    }

    void start() {

        launched = true;
    }

    void stop() {
        launched = false;
    }

    ~Game() {
        delete model;
        delete view;
        delete cursor;
        delete text;
        delete viewBoard;
        delete viewPawns;
        delete viewPlaceEnemy;
        delete viewPlaceDebug;
        delete viewCellsTarget;
        delete viewPointDebug;
        //delete fnt;
    }
};


#endif // GAME_H

