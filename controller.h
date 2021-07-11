#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "types.h"
#include "gamemodel.h"

class Controller
{
    static int8_t id_player;
    bool AI = true;
public:
    Controller(GameModel* model_, bool ai = true, Point startPoint = Point(0, 0), size_t width_place_pawn = 3, size_t height_place_pawn = 3);

    void getPaths(std::map<size_t, std::vector<size_t>>& paths, const cells_t& target_cells);

    bool perform();

    void combinatAlgo(std::map<size_t, std::vector<size_t>>& paths, int* sequence);

    bool nextSet(int* a, int n, int m);

    bool upperStage(size_t& stage);

    /* Если содержимое прошлого хода на территории PawnPlayer.enemyPlace изменилось, то возвращаем тру*/
    bool checkMem();

    /* Сохраним в памяти состояние области врага */
    void saveMem();

private:
    size_t range;
    arr_squars_t memory;
    int temp = 0;
    size_t width;
    size_t height;
    PawnsPlayer* player_pawns;
    GameModel* model;
};


#endif // CONTROLLER_H

