#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "gamemodel.h"
#include "controller.h"
#include "view.h"


/* Класс инкапсулирующий различные объекты в одно целое, следит за созданием удалением объектов */

class Game {
    GameModel* model = nullptr;
    ViewGame* view = nullptr;
    ViewCursor* cursor = nullptr;
    ViewText* text = nullptr;
    ViewBoard* viewBoard = nullptr;
    ViewPawns* viewPawns = nullptr;
    ViewPlaceEnemy* viewPlaceEnemy = nullptr;
    ViewPlaceDebug* viewPlaceDebug = nullptr;
    ViewCellsTarget* viewCellsTarget = nullptr;
    ViewPointDebug* viewPointDebug = nullptr;
    Controller* controller = nullptr;
    Controller* controller2 = nullptr;
    //hgeFont* fnt;
    bool launched = false;

    enum STATE_ORDER_STEP {STATE_STEP_ONE, STATE_STEP_TWO} state_order_state = STATE_STEP_ONE;
    enum STATE_GAME {STATE_GAME_LAUNCH, STATE_GAME_PROCESS, STATE_GAME_END} state_game = STATE_GAME_LAUNCH;

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
    }
    void createPlayers(size_t width, size_t heigth, Point p1, size_t count_player_human = 1) {
        controller = new Controller(model, true, p1, width, heigth);
        if (count_player_human) {
            Cell cell = model->players_pawns.front()->enemyPlace->cells.back();
            Point secondPlayer = Point(cell.x, cell.y);
            controller2 = new Controller(model, false, secondPlayer, width, heigth);
        }
    }


    void updater() {
        model->moveCursore();
        bool win = false;
        switch (state_game)
        {
        case Game::STATE_GAME_LAUNCH:
            model->printString("select an area for the location of the enemy, and without crossing the center", 100, 500);
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


                createPlayers(width, height, target_point, 1);      // 1 - добавить игрока, 0 - без игрока
                state_game = STATE_GAME_PROCESS;
            }
            break;
        case Game::STATE_GAME_PROCESS:
            model->printString("right click skips moves ", 100, 500);

            if (state_order_state == STATE_STEP_ONE) {
                if (controller->perform(win)) {
                    state_order_state = STATE_STEP_TWO;
                    if (win) {
                        state_game = STATE_GAME_END;
                    }
                }
            }
            else {
                if (model->players_pawns.size() == 1 || controller2->perform(win)) {
                    state_order_state = STATE_STEP_ONE;
                    if (win) {
                        state_game = STATE_GAME_END;
                    }
                }
            }
            break;
        case Game::STATE_GAME_END:
            delete controller;
            if (controller2)
                delete controller2;
            break;
        default:
            break;
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
    }
};


#endif // GAME_H

