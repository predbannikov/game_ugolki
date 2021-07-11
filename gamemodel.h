#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include "types.h"


class GameModel : public Observable {
    struct Message {
        float x, y;
        std::string str;
    };
public:
    StateMouse stateMouse;
    Message message;
    Board* board;
    Barrier barrier;
    cells_t cells_debug;
    cells_t cells_target;
    Cell cell_debug;
    std::vector<PawnsPlayer*> players_pawns;


    GameModel(const size_t width_side = COUNT_CELLS_ROW);

    /* Создать фигуры
    player_place - это места пешек для старта игры (для одного игрока)
    */
    PawnsPlayer* createPawns(const PlayerPlace* player_place, const int8_t id_player);

    /* Расставить фигуры на доске*/
    void arrangeFigures(const pawns_t& pawns, const int8_t plyer_side);


    PointAB findPaths(pawns_t& pawns);

    /* Установить размер спрайта для пешки*/
    void setSizePawn(const float& w, const float& h);

    void moveCursore();
    void printString(std::string str, float x = 450, float y = 350);;
    void selectCell();

    /* Получить номера ячеек к которым искать пути и положить их в вектор*/
    void getCellsToSearchPath(std::vector<int>& searchCells, size_t barrier_tolerance);

    void getCellsTarget(cells_t& targetCells, const PawnsPlayer& player, size_t barrier_tolerance, bool inside = true);

    /* Найти короткий путь из from в to*/
    void shortWay(const pawns_t& pawns, int from, int to, std::vector<size_t>& path, int& length);

    /* Передвинуть пешку*/
    int move_pawn(const size_t index_x, const  size_t index_y, const  size_t to_x, const  size_t to_y, PawnsPlayer& player);

    ///* узнать есть ли пешка на клетке из множества pawns*/
    bool this_is_pawn(const pawns_t& pawns, const size_t x, const size_t y) const;

    /* Отработать поведение при выборе пешки */
    bool clickCursor(PawnsPlayer& player);

    /* Для получения простого клика */
    bool clickCursor();


private:
    bool getPawnPlaceOfNumber(const size_t number_pawn, const pawns_t& pawns);

    bool checkEdge(Edge ed, std::vector<Edge>& vec_edge);

    void getIndexs(const size_t numberCell, size_t& x, size_t& y);

    void calcVecEdge(const pawns_t& pawns, std::vector<Edge>& edge);

};





#endif // GAMEMODEL_H

