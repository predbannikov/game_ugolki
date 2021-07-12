#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "types.h"
#include "gamemodel.h"


/* Класс манипулирующий объекрами на сцене */
class Controller
{
    static int8_t id_player;
    bool AI = true;
public:
    Controller(GameModel* model_, bool ai = true, Point startPoint = Point(0, 0), size_t width_place_pawn = 3, size_t height_place_pawn = 3);

    /* Вычисление путей к различным точкам 
    target_cells - точки к которым будут искаться пути 
    paths - ссылка куда буду пути сохраняться*/
    void getPaths(std::map<size_t, std::vector<size_t>>& paths, const cells_t& target_cells);

    /* Вызывается каждый кадр игровым движком*/
    bool perform(bool &win);

    /* Комбинировать различные последовательности вычислений путей - под вопросом, возможно это лишнее*/
    void combinatAlgo(std::map<size_t, std::vector<size_t>>& paths, int* sequence);

    /* Получить следующий набор комбинаций (для комбинаторики)*/
    bool nextSet(int* a, int n, int m);

    /* При вычислении путей, после того как один алгоритм отыграл свою роль переключить на следующий*/
    bool upperStage(size_t& stage);

    /* Сдвинуть пешку с A на B */
    int movePawnAB(size_t a, size_t b);

    /* Если содержимое прошлого хода на территории PawnPlayer.enemyPlace изменилось, то возвращаем тру*/
    bool checkMem();

    /* Сохраним в памяти состояние области врага */
    void saveMem();

private:
    size_t range;   // нужен для хранения длины набора из комбинаций
    arr_squars_t memory;
    int temp = 0;
    size_t width;
    size_t height;
    PawnsPlayer* player_pawns;
    GameModel* model;
};


#endif // CONTROLLER_H

