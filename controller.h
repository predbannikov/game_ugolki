#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "game.h"
#include "gamemodel.h"

class Controller
{
public:
    Controller(GameModel* model_);

    bool processGame();

    void getPaths(std::map<size_t, std::vector<size_t>>& paths, const cells_t& target_cells);

    void process();

    void perform();
private:
    int temp = 0;
    size_t width;
    size_t height;
    PawnsPlayer* player_pawns;
    GameModel* model;
};


#endif // CONTROLLER_H

