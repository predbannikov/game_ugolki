#ifndef GAME_H
#define GAME_H

#include "../../include/hge.h"
#include "../../include/hgefont.h"
#include "../../include/hgegui.h"
#include "../../include/hgecolor.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <thread>
#include "observ.h"



#define COUNT_PAWNS_ROW_INIT        3       //  TODO    Разное колличество в ряд при инициализации пешек
#define COUNT_CELLS_ROW             8       //  TODO    Разное колличество клеток в ряд на доске (возможно и в колонке)
#define COUNT_PAWNS_ONE_SIDE        9       //  TODO    Разное колличество пешек для игроков
#define WIDTH_CELL                  55.0f   //  TODO    Разная ширина клетки
#define WIDTH_PAWN                  42.0f   //  TODO    Разная ширина пешки



// TODO сделать , не тут, логическую проверку, чтоб пешка была меньше поля игровой доски
// TODO никак не отслеживается что место игрока пересекает центр, что при отзеркаливании даёт пересечение мест размещений игроков

extern HGE* hge;
extern hgeFont* fnt;

class Square;
class Pawn;
class Cell;
class PlayerPlace;

/* Тип для массив квадратов*/
using squars_t = std::vector<Square*>;

/* Тип для массива массивов квадратов*/
using arr_squars_t = std::vector<squars_t>;

/* Тип для массива пешек*/
using pawns_t = std::vector<Pawn>;

/* Тип для хранения ячеек*/
using cells_t = std::vector<Cell>;

struct PointF {
    PointF(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
    float x, y;
};

struct Point {
    Point(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    int x, y;
    Point operator-(Point& p) {
        return Point(this->x - p.x, this->y - p.y);
    }
};

struct PointAB {
    PointAB(int _a = 0, int _b = 0) : a(_a), b(_b) {}
    int a, b;
};

/* Ячейка, имеет порядковый номер и координаты в декартовой системе*/
struct Cell {
    Cell(size_t _x = 0, size_t _y = 0, size_t _n = 0) : x(_x), y(_y), n(_n) {}
    size_t x;
    size_t y;
    size_t n;       // Порядковый номер на сцене
};

/* Квадрат, еденица игрового поля
filled - квадрат содержит пешку*/
struct Square : public Cell {
    Square(size_t _x, size_t _y, size_t _n, bool _filled = false) : Cell(_x, _y, _n), filled(_filled) {}
    PointF leftTop;
    PointF rightTop;
    PointF leftBottom;
    PointF rightBottom;
    bool filled;
};

/* Пешка
place - фигура выполнила свою рабту (достигла своей задачи/цели)
quad - полигон или текстура для отображения пешки*/
struct Pawn : public Cell {
    Pawn(size_t _x = 0, size_t _y = 0, size_t _n = 0) : Cell(_x, _y, _n) {
        spr_pawn = new hgeSprite(0, 0, 0, 16, 16);              // Размер пешки регулируется в конструкторе контроллера
        hgeColorRGB cur_color(0, 0, 1, 1);
        spr_pawn->SetColor(cur_color.GetHWColor());
    };
    bool place = false;
    hgeSprite* spr_pawn;
};

/* Игровая доска*/
struct Board {
    Board(size_t count_items_side = COUNT_CELLS_ROW, float width_item = WIDTH_CELL) {
        float tmp_x = 0;
        float tmp_y = 0;
        size_t counter = 0;
        arr_squars.resize(count_items_side);
        squars.resize(count_items_side * count_items_side);
        for (size_t i = 0; i < count_items_side; i++) {
            squars_t squarsTmp(count_items_side);
            for (size_t j = 0; j < count_items_side; j++) {
                Square* squar = new Square(j, i, i * count_items_side + j);
                squar->leftTop.x = tmp_x;
                squar->leftTop.y = tmp_y;

                squar->rightTop.x = tmp_x + width_item;
                squar->rightTop.y = tmp_y;

                squar->rightBottom.x = tmp_x + width_item;
                squar->rightBottom.y = tmp_y + width_item;

                squar->leftBottom.x = tmp_x;
                squar->leftBottom.y = tmp_y + width_item;
                squars[squar->n] = squar;
                squarsTmp[j] = squar;
                tmp_x += width_item;
            }
            tmp_y += width_item;
            arr_squars[i] = squarsTmp;
            tmp_x = 0;
        }
    }
    squars_t squars;
    arr_squars_t arr_squars;
};

/* Расчитать игровые поля пешек игрока, найти наиболее интересные поля */
class PlayerPlace {
public:
    PlayerPlace(const Board* board, Point pointLeftTopCorner, const size_t width = 3, const size_t height = 3) {
        width_board = board->arr_squars.front().size();
        height_board = board->arr_squars.size();
        while (pointLeftTopCorner.x + width > board->arr_squars.front().size()) {  // Если нестыковка и выходим за границу, то правим точку как можем
            pointLeftTopCorner.x--;
        }
        while (pointLeftTopCorner.y + height > board->arr_squars.size()) {         // Если нестыковка и выходим за границу, то правим точку как можем
            pointLeftTopCorner.y--;
        }
        std::vector<size_t>* vec = new std::vector<size_t>;
        for (size_t i = pointLeftTopCorner.y; i < pointLeftTopCorner.y + height; i++) {
            for (size_t j = pointLeftTopCorner.x; j < pointLeftTopCorner.x + width; j++) {
                cells.push_back(Cell(j, i, i * board->arr_squars.front().size() + j));
            }
        }
        // Угловые точки и середина (most interesting points left top) ....
        Point miplt(cells.front().x, cells.front().y);
        Point miprt(cells.front().x + width - 1, cells.front().y);
        Point miprb(cells.front().x + width - 1, cells.front().y + height - 1);
        Point miplb(cells.front().x, cells.front().y + height - 1);

        Point p1 = miplt - Point(0, 0);
        Point p2 = miprt - Point(board->arr_squars.front().size() - 1, 0);
        Point p3 = miprb - Point(board->arr_squars.front().size() - 1, board->arr_squars.size() - 1);
        Point p4 = miplb - Point(0, board->arr_squars.size() - 1);

        std::map<double, Point> minDistance;
        minDistance.insert({ sqrt(p1.x * p1.x + p1.y * p1.y), miplt });
        minDistance.insert({ sqrt(p2.x * p2.x + p2.y * p2.y), miprt });
        minDistance.insert({ sqrt(p3.x * p3.x + p3.y * p3.y), miprb });
        minDistance.insert({ sqrt(p4.x * p4.x + p4.y * p4.y), miplb });

        Point mip = minDistance.begin()->second;
        most_interest_point = mip;

        Point mip_middle(cells.front().x + width / 2, cells.front().y + height / 2);
        Point remaind(width % 2, height % 2);


        // TODOСделать авто определение что пешки игрока находятся строго по центру стороны доски
        // наиболее интересующая точка будет по центру а не с угла
        
        //if (!remaind.x) {
        //    if (board->arr_squars.front().size() / 2 > mip.x) {
        //        mip.x--;
        //    }
        //    else {
        //        mip.x++;
        //    }
        //}

        //if (!remaind.y) {
        //    if (board->arr_squars.size() / 2 > mip.y) {
        //        mip.y--;
        //    }
        //    else {
        //        mip.y++;
        //    }
        //}
        // Оставил для отладки
        //str = std::string("1-" + std::to_string(miplt.x) + ":" + std::to_string(miplt.y) + "\n" + "2-" + std::to_string(miprt.x) + ":" + std::to_string(miprt.y) + "\n" + "3-" + std::to_string(miprb.x) + ":" + std::to_string(miprb.y) + "\n" + "4-" + std::to_string(miplb.x) + ":" + std::to_string(miplb.y) + "\n" + "middle-" + std::to_string(mip_middle.x) + ":" + std::to_string(mip_middle.y) + "\n" + "remaind-" + std::to_string(remaind.x) + ":" + std::to_string(remaind.y) + "\n" + "mip = " + std::to_string(mip.x) + ":" + std::to_string(mip.y) + "\n");
    }

    PlayerPlace* getMirrorPlace() const {
        PlayerPlace* player = new PlayerPlace(*this);
        cells_t mirrorPlace;
        for (size_t i = 0; i < this->cells.size(); i++) {
            size_t x = width_board - 1 - this->cells[i].x;
            size_t y = height_board - 1 - this->cells[i].y;
            size_t n = width_board * y + x;
            player->cells[i] = Cell(x, y, n);
        }
        player->most_interest_point = Point(width_board - 1 - this->most_interest_point.x, height_board - 1 - this->most_interest_point.y);
        return player;
    }
    // 
    Point most_interest_point;
    size_t width_board;
    size_t height_board;
    cells_t cells;
    std::string str;
};

/* Пешки игрока/AI */
struct PawnsPlayer {
    pawns_t pawns;
    const PlayerPlace* playerPlace;
    const PlayerPlace* enemyPlace;

    //const std::vector<size_t> *pawn_place;

    void init_loc_pawn(const cells_t& cells_player) {
        for (size_t i = 0; i < cells_player.size(); i++) {
            pawns.push_back(Pawn(cells_player[i].x, cells_player[i].y, cells_player[i].n));
        }
    }

    PawnsPlayer(const PlayerPlace* player_place) : playerPlace(player_place) {
        playerPlace = player_place;
        //pawn_place = player_place;
        pawns.reserve(player_place->cells.size());
        init_loc_pawn(player_place->cells);
        enemyPlace = player_place->getMirrorPlace();
    }
};


/* Ребро графа
по умолчанию длина равная 1 (int)*/
struct Edge {
    int a, b;
    int cost = 1;
};

struct Barrier {
    size_t currentLvlBarrier = 1;
    double maxLvlBarrier = 1.1f;
    size_t maxFillLvl = 1;
    size_t currentFill = 0;

};

class Game {
    GameModel* game;
    ViewGame* view;
    ViewCursor* cursor;
    Controller* controller;
    ViewText* text;
    ViewBoard* viewBoard;
    ViewPawns* viewPawns;
    ViewPlaceEnemy* viewPlaceEnemy;
    ViewPlaceDebug* viewPlaceDebug;
    ViewCellsTarget* viewCellsTarget;
    ViewPointDebug* viewPointDebug;
    hgeFont* fnt;

public:
    Game() {
        // TODO создать контроллер с указанием кто им будет играть
        // 
    }
    ~Game() {
        delete game;
        delete view;
        delete cursor;
        delete controller;
        delete text;
        delete viewBoard;
        delete viewPawns;
        delete viewPlaceEnemy;
        delete viewPlaceDebug;
        delete viewCellsTarget;
        delete viewPointDebug;
        delete fnt;
    }
};



#endif // GAME_H

