
#pragma once

#include "../../include/hge.h"
#include "../../include/hgefont.h"
#include "../../include/hgegui.h"
#include "../../include/hgecolor.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
#include <map>



#define COUNT_PAWNS_ROW_INIT        3       //  TODO    Разное колличество в ряд при инициализации пешек
#define COUNT_CELLS_ROW             8       //  TODO    Разное колличество клеток в ряд на доске (возможно и в колонке)
#define COUNT_PAWNS_ONE_SIDE        9       //  TODO    Разное колличество пешек для игроков
#define WIDTH_CELL                  55.0f   //  TODO    Разная ширина клетки
#define WIDTH_PAWN                  42.0f   //  TODO    Разная ширина пешки

extern HGE* hge;
extern hgeFont* fnt;

class Square;
class Pawn;

/*Тип для массив квадратов*/
using squars_t = std::vector<Square*>;

/*Тип для массива массивов квадратов*/
using arr_squars_t = std::vector<squars_t>;

/*Тип для массива пешек*/
using pawns_t = std::vector<Pawn>;

struct PointF { 
    PointF(float _x = 0.0f, float _y = 0.0f): x(_x), y(_y){}
    float x;
    float y;
};

struct PointAB {
    PointAB(int _a = 0, int _b = 0) : a(_a), b(_b) {}
    int a;
    int b;
};

/*Ячейка, имеет порядковый номер и координаты в декартовой системе*/
struct Cell {
    Cell(size_t _x, size_t _y, size_t _n) : x(_x), y(_y), n(_n) {}
    size_t x;
    size_t y;
    size_t n;       // Порядковый номер на сцене
};

/*Квадрат, еденица игрового поля
filled - квадрат содержит пешку*/
struct Square : public Cell {
    Square(size_t _x, size_t _y, size_t _n, bool _filled = false) : Cell(_x, _y, _n), filled(_filled){}
    PointF leftTop;
    PointF rightTop;
    PointF leftBottom;
    PointF rightBottom;
    bool filled;
};

/*Пешка
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

/*Пешки*/
struct PawnsPlayer {
    enum LOC_INIT { LOC_INIT_LEFT_TOP, LOC_INIT_RIGHT_BOTTOM };
    std::vector<LOC_INIT> loc_init_vec;                                 // TODO Сделать статик
    const size_t& count_items;
    pawns_t pawns;

    void init_left_top() {
        for (size_t i = 0; i < COUNT_PAWNS_ROW_INIT; i++) {
            for (size_t j = 0; j < COUNT_PAWNS_ROW_INIT; j++) {
                pawns.push_back(Pawn(i, j, i * COUNT_CELLS_ROW + j));
            }
        }
    }

    void init_right_bottom() {
        for (size_t i = COUNT_CELLS_ROW - 1; i > COUNT_CELLS_ROW - 1 - COUNT_PAWNS_ROW_INIT; i--) {
            for (size_t j = COUNT_CELLS_ROW - 1; j > COUNT_CELLS_ROW - 1 - COUNT_PAWNS_ROW_INIT; j--) {
                pawns.push_back(Pawn(i, j, i * COUNT_CELLS_ROW + j));
            }
        }
    }
    void init(LOC_INIT loc) {
        std::for_each(loc_init_vec.begin(), loc_init_vec.end(), [&loc](const LOC_INIT l) {      //TODO Переделать на обычный цикл, чтоб избавиться от enum unscoped
            if (l == loc) throw std::logic_error("this pawns alreasy exists");
        });
        switch (loc)
        {
        case PawnsPlayer::LOC_INIT_LEFT_TOP:
            loc_init_vec.push_back(loc);
            init_left_top();
            break;
        case PawnsPlayer::LOC_INIT_RIGHT_BOTTOM:
            loc_init_vec.push_back(loc);
            init_right_bottom();
            break;
        default:
            break;
        }
    }
    PawnsPlayer(const size_t _count_items) :count_items(_count_items) {
        pawns.reserve(count_items);
    }

};

//class Player {
//public:
//    Pawns* pawns;
//    Player(Pawns* _pawns) : pawns(_pawns) {}
//};

/*Ребро графа 
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

class Observer
{
public:
    virtual void update() = 0;
};

class Observable
{
public:
    void addObserver(Observer* observer)
    {
        _observers.push_back(observer);
    }
    void notifyUpdate()
    {
        int size = _observers.size();
        for (int i = 0; i < size; i++)
        {
            _observers[i]->update();
        }
    }
private:
    std::vector<Observer*> _observers;
};

class GameModel : public Observable{

    /*Игровая доска*/
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
                    Square* squar = new Square(i, j, i*count_items_side + j);
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
                    tmp_y += width_item;
                }
                arr_squars[i] = squarsTmp;
                tmp_x += width_item;
                tmp_y = 0;
            }      
        }
        squars_t squars;
        arr_squars_t arr_squars;
    };
    struct StateChoice {
        bool mousePressed = false;
        float pressedTime = 0;
        const float deltaForDifferent = 0.15f;
        float mx, my;
        int x, y;
        bool choice = false;
    };
    struct Message {
        float x, y;
        std::string str;
    };
public:
    StateChoice stateMouse;
    Message message;
    Board *board;
    Barrier barrier;

    std::vector<PawnsPlayer*> players_pawns;

    GameModel() {
        board = new Board(COUNT_CELLS_ROW);

    };

    PawnsPlayer* createPawns(const PawnsPlayer::LOC_INIT loc, const size_t count_pawns = COUNT_PAWNS_ONE_SIDE) {
        PawnsPlayer* pawns = new PawnsPlayer(count_pawns);
        try
        {
            pawns->init(loc);
        }
        catch (const std::exception&)
        {
            delete pawns;
            printString("error init pawns");
            return nullptr;
        }
        players_pawns.push_back(pawns);
        return pawns;
    }

    /*Расставить фигуры на доске*/
    void arrangeFigures(const pawns_t& pawns) {
        for (size_t i = 0; i < pawns.size(); i++) {
            board->arr_squars[pawns[i].x][pawns[i].y]->filled = true;
        }
    }

    PointAB findPaths(pawns_t &pawns) {
        std::vector<std::vector<int>> mapPawns(COUNT_CELLS_ROW, std::vector<int>(COUNT_CELLS_ROW, 0));

        std::map<double, std::vector<size_t>> paths;


        size_t barrier_tolerance = 0;
        while (paths.empty()) {
            if (barrier_tolerance != 0)
                std::cout << "";
            std::vector<int> numbersCellsWins;
            if (barrier_tolerance == COUNT_CELLS_ROW) {
                std::cout << "pad";
                return PointAB();
            }
            getCellsToSearchPath(numbersCellsWins, barrier_tolerance);    // Получаем ячейки для поиска
            for (size_t i = 0; i < pawns.size(); i++) {
                if (!pawns[i].place)
                    for (size_t j = 0; j < numbersCellsWins.size(); j++) {
                        std::vector<size_t> path;
                        double length = -1;
                        shortWay(pawns, pawns[i].n, numbersCellsWins[j], path, length);
                        if (length > 0)
                            paths[length] = path;
                    }
            }
            barrier_tolerance++;
            if (!paths.empty()) {
                std::cout << "";
                if (barrier_tolerance < barrier.currentLvlBarrier) {
                    for (size_t i = 0; i < pawns.size(); i++)
                        pawns[i].place = false;
                    paths.clear();
                    barrier_tolerance = 0;
                    barrier.currentLvlBarrier = barrier_tolerance;
                }
            }
        }

        barrier.currentLvlBarrier = barrier_tolerance;

        if (paths.size() == 1)
            std::cout << "";
        size_t x, y, x_to, y_to;
        auto it = paths.begin()->second.begin();
        getIndexs(*it, x, y);
        getIndexs(*(it + 1), x_to, y_to);
        //directStepPawn(*it, *(it + 1));
        int ret = move_pawn(x, y, x_to, y_to, pawns);
        if (*it == 55)
            std::cout << "";
        if (paths.begin()->first < 1.1f) {
            pawns[ret].place = true;
        }

        //if (checkWin()) {
        //    std::cout << "WIN";
        //    cycle = false;;
        //}
        return PointAB();
    }

    void setSizePawn(const float& w, const float &h) {
        for(size_t p = 0; p < players_pawns.size(); p++)
            for (size_t i = 0; i < players_pawns[p]->pawns.size(); i++) {
                players_pawns[p]->pawns[i].spr_pawn->SetTextureRect(0, 0, w, h);
            }
    }

    void updateCursor() {
        hge->Input_GetMousePos(&stateMouse.mx, &stateMouse.my);
        notifyUpdate();
    }
    void printString(std::string str, float x = 350, float y = 350) {
        message.x = x;
        message.y = y;
        message.str = str;
        notifyUpdate();
    };
    void selectCell() {
        notifyUpdate();
    }
private:

    // TODO На данный момент всё завязано статически, ищутся ячейки ближайшие к правому нижнему углу
    /*Получить номера ячеек к которым искать пути и положить их в вектор*/
    void getCellsToSearchPath(std::vector<int>& searchCells, size_t barrier_tolerance) {
        for (int i = COUNT_CELLS_ROW - 1; i >= COUNT_CELLS_ROW - barrier_tolerance; i--) {
            for (int j = COUNT_CELLS_ROW - 1; j >= COUNT_CELLS_ROW - barrier_tolerance; j--) {
                if (!board->arr_squars[i][j]->filled)
                    searchCells.push_back(i * COUNT_CELLS_ROW + j);
            }
        }
    }

    void shortWay(const pawns_t &pawns, int from, int to, std::vector<size_t>& path, double& length) {
        std::vector<Edge> vec_edge;
        calcVecEdge(pawns, vec_edge);

        int number_vert_to_search = to;
        int number_vert_from_search = from;
        int n, m, v;
        const int inf = 1000000000;

        n = board->squars.size();
        m = vec_edge.size();

        std::vector<int> d(n, inf);
        v = number_vert_from_search;
        d[v] = 0;
        std::vector<int> p(n, -1);
        for (;;) {
            bool any = false;
            for (int j = 0; j < m; ++j)
                if (d[vec_edge[j].a] < inf)
                    if (d[vec_edge[j].b] > d[vec_edge[j].a] + vec_edge[j].cost) {
                        d[vec_edge[j].b] = d[vec_edge[j].a] + vec_edge[j].cost;
                        p[vec_edge[j].b] = vec_edge[j].a;
                        any = true;
                    }
            if (!any)  break;
        }

        int t = number_vert_to_search; // искомая вершина
        //std::vector<size_t>* path = new std::vector<size_t>;
        if (d[t] == inf) {
            fnt->printf(170, 10, HGETEXT_LEFT, "no path from %i to %i", v, t);
        }
        else {
            length = d[t];
            //std::vector<int> path;
            for (int cur = t; cur != -1; cur = p[cur]) {
                path.push_back(cur);

            }
            std::reverse(path.begin(), path.end());

            fnt->printf(170, 10, HGETEXT_LEFT, "path from %i to %i :", v, t);
            float axes_y = 25;
            for (size_t i = 0; i < path.size(); ++i) {
                fnt->printf(170, axes_y, HGETEXT_LEFT, "%i :", path.at(i));
                axes_y += 13;
            }
        }
    }

    bool getPawnPlaceOfNumber(const size_t number_pawn, const pawns_t& pawns) {
        size_t index_x, index_y;
        getIndexs(number_pawn, index_x, index_y);
        for (size_t i = 0; i < pawns.size(); i++) {
            if (pawns[i].x == index_x && pawns[i].y == index_y) {
                return pawns[i].place;
            }
        }
        return false;
    }

    bool checkEdge(Edge ed, std::vector<Edge>& vec_edge) {
        for (size_t i = 0; i < vec_edge.size(); i++)
            if (ed.a == vec_edge[i].a && ed.b == vec_edge[i].b)
                return true;
        return false;
    }

    void getIndexs(const size_t numberCell, size_t& x, size_t& y) {
        x = numberCell / COUNT_CELLS_ROW;
        y = numberCell % COUNT_CELLS_ROW;
    }

    void calcVecEdge(const pawns_t &pawns, std::vector<Edge>& edge) {
        for (int i = 0; i < COUNT_CELLS_ROW; i++) {
            for (int j = 0; j < COUNT_CELLS_ROW; j++) {
                Edge ed;
                ed.cost = 1.0f;
                ed.a = j + i * COUNT_CELLS_ROW;
                if (j == 3)
                    std::cout << "";
                if (getPawnPlaceOfNumber(ed.a, pawns))
                    continue;
                if (j - 1 >= 0 && !board->arr_squars[i][j - 1]->filled) {
                    ed.b = j - 1 + i * COUNT_CELLS_ROW;
                    edge.push_back(ed);
                }
                if (j + 1 < COUNT_CELLS_ROW && !board->arr_squars[i][j + 1]->filled) {
                    ed.b = i * COUNT_CELLS_ROW + j + 1;
                    edge.push_back(ed);
                }
                if (i - 1 >= 0 && !board->arr_squars[i - 1][j]->filled) {
                    ed.b = (i - 1) * COUNT_CELLS_ROW + j;
                    edge.push_back(ed);
                }
                if (i + 1 < COUNT_CELLS_ROW && !board->arr_squars[i + 1][j]->filled) {
                    ed.b = (i + 1) * COUNT_CELLS_ROW + j;
                    edge.push_back(ed);
                }
            }
        }

    }

    int move_pawn(const size_t index_x, const  size_t index_y, const  size_t to_x, const  size_t to_y, pawns_t& pawns) {
        for (size_t i = 0; i < pawns.size(); i++) {
            if (pawns[i].x == index_x && pawns[i].y == index_y) {
                pawns[i].x = to_x;
                pawns[i].y = to_y;
                pawns[i].n = to_x * COUNT_CELLS_ROW + to_y;
                board->arr_squars[index_x][index_y]->filled = false;
                board->arr_squars[to_x][to_y]->filled = true;
                return i;
            }
        }
        return -1;
    }

};

struct ViewBoard : public Observer {
    ViewBoard(GameModel* model) {
        game = model;
        game->addObserver(this);
    }
    void renderQuad(const PointF &p1, const PointF &p2, const PointF &p3, const PointF &p4, int color = 0x00FF0000) {
        hge->Gfx_RenderLine(p1.x, p1.y, p2.x, p2.y, color);
        hge->Gfx_RenderLine(p2.x, p2.y, p3.x, p3.y, color);
        hge->Gfx_RenderLine(p3.x, p3.y, p4.x, p4.y, color);
        hge->Gfx_RenderLine(p4.x, p4.y, p1.x, p1.y, color);
    }

    virtual void update() {
        const arr_squars_t &squars = game->board->arr_squars;
        for (int i = 0; i < squars.size(); i++) {
            for (int j = 0; j < squars.size(); j++) {
                renderQuad(squars[i][j]->leftTop, squars[i][j]->rightTop, squars[i][j]->rightBottom, squars[i][j]->leftBottom, 0xFFFFFFFF);
                fnt->printf(squars[i][j]->leftTop.x, squars[i][j]->leftTop.y, HGETEXT_LEFT, " %i", i * squars[i].size() + j);               // TODO перенести во вьюшку
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
        // TODO сделать , не тут, логическую проверку, чтоб пешка была меньше поля игровой доски
        const float dwBoarder = width_cell - pawns.front().spr_pawn->GetWidth();
        const float dhBoarder = width_cell - pawns.front().spr_pawn->GetHeight();
        for (int i = 0; i < pawns.size(); i++) {
            pawns[i].spr_pawn->Render(dwBoarder /2. + pawns[i].x * width_cell, dhBoarder / 2. + pawns[i].y * width_cell);
        }
    }
    GameModel* game;
};

struct ViewGame : public Observer{
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

class Controller
{
public:
    Controller(GameModel* model_) : model(model_)
    {
        player_pawns = model->createPawns(PawnsPlayer::LOC_INIT_LEFT_TOP);
        model->arrangeFigures(player_pawns->pawns);
        model->setSizePawn(WIDTH_PAWN, WIDTH_PAWN);
    }
    void perform()
    {
        model->findPaths(player_pawns->pawns);
        model->printString("hello MVC", 450, 350);
        if (hge->Input_IsMouseOver()) {
            model->updateCursor();
        }
    }
private:
    PawnsPlayer* player_pawns;
    GameModel* model;
};

























class Cell_ {
    struct Point {
        float x, y;
    };
public:
    Point leftTop;
    Point rightTop;
    Point leftBottom;
    Point rightBottom;
};

class Game {
    struct PawnOld;
    using pawns_t = std::vector<PawnOld>;
    using cells_t = std::vector<std::vector<Cell_>>;

    struct StateChoice {
        bool mousePressed = false;
        float pressedTime = 0;
        const float deltaForDifferent = 0.15f;
        int x, y;
        bool choice = false;
    };

    struct PawnOld {
    public:
        int x;
        int y;
        bool place = false;
        size_t number;
        hgeQuad* quad = nullptr;
    };

    bool state_mouse_over = false;
    float dtime = 0;
    float width_cell;
    int count_cell_x;
    int count_cell_y;
    float border;
    cells_t cells;
    pawns_t enemy;
    pawns_t player;
    StateChoice stateCursor;

    //Pawn pawn;
    std::string str;
    bool cycle = true;

    enum STATE_STEP { STATE_STEP_ENEMY, STATE_STEP_PLAYER } state_step = STATE_STEP_ENEMY;
public:

    Game(float width = 10.0f, int x = 8, int y = 8) : width_cell(width), count_cell_x(x), count_cell_y(y) {
        cells.resize(x);
        for (int i = 0; i < x; i++)
            cells[i].resize(y);
        float tmp_x = 0;
        float tmp_y = 0;
        for (int i = 0; i < cells.size(); i++) {
            for (int j = 0; j < cells.size(); j++) {
                cells[i][j].leftTop.x = tmp_x;
                cells[i][j].leftTop.y = tmp_y;

                cells[i][j].rightTop.x = tmp_x + width;
                cells[i][j].rightTop.y = tmp_y;

                cells[i][j].rightBottom.x = tmp_x + width;
                cells[i][j].rightBottom.y = tmp_y + width;

                cells[i][j].leftBottom.x = tmp_x;
                cells[i][j].leftBottom.y = tmp_y + width;
                tmp_y += width;
            }
            tmp_x += width;
            tmp_y = 0;
        }
        set_top_pawn(enemy);
        set_bottom_pawn(player);

        createVectorPoint();
    }
    //****************************************************************************
    //****************************************************************************
    //****************************************************************************


    struct PointCell {
        size_t n;
        size_t index_x;
        size_t index_y;
        float x;
        float y;
    };

#define COUNT_RANGE 8
    std::vector<PointCell> vec_point;

    void createVectorPoint() {
        size_t counter = 0;
        for (size_t i = 0; i < COUNT_RANGE; i++) {
            for (size_t j = 0; j < COUNT_RANGE; j++) {
                PointCell cell;
                cell.n = counter;
                cell.index_x = i;
                cell.index_y = j;
                cell.x = i * width_cell + width_cell / 2.;
                cell.y = j * width_cell + width_cell / 2.;
                vec_point.push_back(cell);
            }
        }

    }

    void calcVecEdge(std::vector<Edge>& edge, const std::vector<std::vector<int>>& mapPawns) {
        for (int i = 0; i < COUNT_RANGE; i++) {
            for (int j = 0; j < COUNT_RANGE; j++) {
                Edge ed;
                ed.cost = 1.0f;
                ed.a = j + i * COUNT_RANGE;
                if (j == 3)
                    std::cout << "";
                if (getPawnPlaceOfNumber(ed.a, enemy))
                    continue;
                if (j - 1 >= 0 && !mapPawns[i][j - 1]) {
                    ed.b = j - 1 + i * COUNT_RANGE;
                    edge.push_back(ed);
                }
                if (j + 1 < COUNT_RANGE && !mapPawns[i][j + 1]) {
                    ed.b = i * COUNT_RANGE + j + 1;
                    edge.push_back(ed);
                }
                if (i - 1 >= 0 && !mapPawns[i - 1][j]) {
                    ed.b = (i - 1) * COUNT_RANGE + j;
                    edge.push_back(ed);
                }
                if (i + 1 < COUNT_RANGE && !mapPawns[i + 1][j]) {
                    ed.b = (i + 1) * COUNT_RANGE + j;
                    edge.push_back(ed);
                }
            }
        }
        
    }

    void shortWay(int from, int to, std::vector<size_t>& path, double& length, const std::vector<std::vector<int>>& mapPawns) {
        std::vector<Edge> vec_edge;
        calcVecEdge(vec_edge, mapPawns);

        int number_vert_to_search = to;
        int number_vert_from_search = from;
        int n, m, v;
        const int inf = 1000000000;

        n = vec_point.size();
        m = vec_edge.size();

        std::vector<int> d(n, inf);
        v = number_vert_from_search;
        d[v] = 0;
        std::vector<int> p(n, -1);
        for (;;) {
            bool any = false;
            for (int j = 0; j < m; ++j)
                if (d[vec_edge[j].a] < inf)
                    if (d[vec_edge[j].b] > d[vec_edge[j].a] + vec_edge[j].cost) {
                        d[vec_edge[j].b] = d[vec_edge[j].a] + vec_edge[j].cost;
                        p[vec_edge[j].b] = vec_edge[j].a;
                        any = true;
                    }
            if (!any)  break;
        }

        int t = number_vert_to_search; // искомая вершина
        //std::vector<size_t>* path = new std::vector<size_t>;
        if (d[t] == inf) {
            fnt->printf(170, 10, HGETEXT_LEFT, "no path from %i to %i", v, t);
        }
        else {
            length = d[t];
            //std::vector<int> path;
            for (int cur = t; cur != -1; cur = p[cur]) {
                path.push_back(cur);

            }
            std::reverse(path.begin(), path.end());

            fnt->printf(170, 10, HGETEXT_LEFT, "path from %i to %i :", v, t);
            float axes_y = 25;
            for (size_t i = 0; i < path.size(); ++i) {
                fnt->printf(170, axes_y, HGETEXT_LEFT, "%i :", path.at(i));
                axes_y += 13;
            }
        }
    }
    
    struct Barrier {
        size_t currentLvlBarrier = 1;
        double maxLvlBarrier = 1.1f;
        size_t maxFillLvl = 1;
        size_t currentFill = 0;

    };
    
    Barrier barrier;

    void findPaths() {
        std::vector<std::vector<int>> mapPawns(COUNT_RANGE, std::vector<int>(COUNT_RANGE, 0));
        remapPawns(mapPawns);

        std::map<double, std::vector<size_t>> paths;


        size_t barrier_tolerance = 0;
        while (paths.empty()) {
            if (barrier_tolerance != 0)
                std::cout << "";
            std::vector<int> numbersCellsWins;
            if (barrier_tolerance == COUNT_RANGE) {
                std::cout << "pad";
                return;
            }
            getCellsToSearchPath(numbersCellsWins, mapPawns, barrier_tolerance);
            for (size_t i = 0; i < enemy.size(); i++) {
                if (!enemy[i].place)
                    for (size_t j = 0; j < numbersCellsWins.size(); j++) {
                        std::vector<size_t> path;
                        double length = -1;
                        shortWay(enemy[i].number, numbersCellsWins[j], path, length, mapPawns);
                        if (length > 0)
                            paths[length] = path;
                    }
            }
            barrier_tolerance++;
            if (!paths.empty()) {
                std::cout << "";
                if (barrier_tolerance < barrier.currentLvlBarrier) {
                    for (size_t i = 0; i < enemy.size(); i++)
                        enemy[i].place = false;
                    paths.clear();
                    barrier_tolerance = 0;
                    barrier.currentLvlBarrier = barrier_tolerance;
                }     
            }
        }

        barrier.currentLvlBarrier = barrier_tolerance;

        if (paths.size() == 1)
            std::cout << "";
        size_t x, y, x_to, y_to;
        auto it = paths.begin()->second.begin();
        getIndexs(*it, x, y);
        getIndexs(*(it + 1), x_to, y_to);
        //directStepPawn(*it, *(it + 1));
        int ret = move_pawn(x, y, x_to, y_to, enemy);
        if (*it == 55)
            std::cout << "";
        if (paths.begin()->first < 1.1f) {
            enemy[ret].place = true;
        }

        if (checkWin()) {
            std::cout << "WIN";
            cycle = false;;
        }
    }

    bool checkWin() {
        bool check = true;
        for (size_t i = 0; i < enemy.size(); i++) {
            if (!enemy[i].place) {
                check = false;
            }
        }
        if (check) {
            printState("ENEMY WIN!");
        }
        check = true;
        for (size_t i = 0; i < player.size(); i++) {
            if (player[i].x >= 3 || player[i].y >= 3) {
                check = false;
            }    
        }
        if (check) {
            printState("PLAYER WIN!");
        }
        return check;
    }

    void getCellsToSearchPath(std::vector<int>& searchCells, const std::vector<std::vector<int >>& mapPawns, size_t barrier_tolerance) {
        for (int i = COUNT_RANGE - 1; i >= COUNT_RANGE - barrier_tolerance; i--) {
            for (int j = COUNT_RANGE - 1; j >= COUNT_RANGE - barrier_tolerance; j--) {
                if (!mapPawns[i][j])
                    searchCells.push_back(i * COUNT_RANGE + j);
            }
        }
    }

    //int directStepPawn(const size_t a, const size_t b) {
    //    int a_x, a_y, b_x, b_y;
    //    a_x = a / COUNT_RANGE;
    //    a_y = a % COUNT_RANGE;
    //    b_x = a / COUNT_RANGE;
    //    b_y = a % COUNT_RANGE;
    //    int lvl = 1;
    //    for (int i = 0; i < COUNT_RANGE; i++) {
    //        if (a_x == b_x && a_y < i && b_y < i
    //            || a_y == b_y && a_x < i && b_x < i) {

    //             std::cout << "";
    //             printState("lvl = " + std::to_string(i));
    //             return 0;
    //       }
    //    }
    //}

    void remapPawns(std::vector<std::vector<int>>& mapPawns) {
        for (size_t i = 0; i < player.size(); i++) {
            mapPawns[enemy[i].x][enemy[i].y] = 1;
            mapPawns[player[i].x][player[i].y] = 1;
        }
    }

    bool getPawnPlaceOfNumber(const size_t number_pawn, pawns_t& pawns) {
        size_t index_x, index_y;
        getIndexs(number_pawn, index_x, index_y);
        for (size_t i = 0; i < pawns.size(); i++) {
            if (pawns[i].x == index_x && pawns[i].y == index_y) {
                return pawns[i].place;
            }
        }
        return false;
    }

    bool checkEdge(Edge ed, std::vector<Edge>& vec_edge) {
        for (size_t i = 0; i < vec_edge.size(); i++)
            if (ed.a == vec_edge[i].a && ed.b == vec_edge[i].b)
                return true;
        return false;
    }

    void getIndexs(const size_t numberCell, size_t& x, size_t& y) {
        x = numberCell / COUNT_RANGE;
        y = numberCell % COUNT_RANGE;
    }

    void set_top_pawn(pawns_t& pawns, int count_pawn = 9) {
        pawns.resize(count_pawn);
        size_t counter_pawn = 0;
        for (size_t i = 0; i < 3; i++) {
            for (size_t j = 0; j < 3; j++) {
                pawns[counter_pawn].x = i;
                pawns[counter_pawn].y = j;
                hgeColorRGB hgeCol(0.6, 0.6, 0.6, 1.0);
                pawns[counter_pawn].quad = createQuad(i, j, 3, hgeCol.GetHWColor());
                pawns[counter_pawn].number = i * COUNT_RANGE + j;
                counter_pawn++;
            }
        }
    }

    void set_bottom_pawn(pawns_t& pawns, int count_pawn = 9) {
        pawns.resize(count_pawn);
        size_t counter_pawn = 8;
        for (size_t i = 7; i > 7 - 3; i--) {
            for (size_t j = 7; j > 7 - 3; j--) {
                pawns[counter_pawn].x = i;
                pawns[counter_pawn].y = j;
                pawns[counter_pawn].quad = createQuad(i, j);
                counter_pawn--;
            }
        }
    }

    void renderCell(int i, int j, int color = 0x00FF0000) {
        hge->Gfx_RenderLine(cells[i][j].leftTop.x, cells[i][j].leftTop.y, cells[i][j].rightTop.x, cells[i][j].rightTop.y, color);
        hge->Gfx_RenderLine(cells[i][j].rightTop.x, cells[i][j].rightTop.y, cells[i][j].rightBottom.x, cells[i][j].rightBottom.y, color);
        hge->Gfx_RenderLine(cells[i][j].rightBottom.x, cells[i][j].rightBottom.y, cells[i][j].leftBottom.x, cells[i][j].leftBottom.y, color);
        hge->Gfx_RenderLine(cells[i][j].leftBottom.x, cells[i][j].leftBottom.y, cells[i][j].leftTop.x, cells[i][j].leftTop.y, color);
        fnt->printf(cells[i][j].leftTop.x, cells[i][j].leftTop.y, HGETEXT_LEFT, " %i", i * cells.size() + j);

    }

    hgeQuad* createQuad(int i, int j, float border = 1.0f, int color = 0xFFFFFFFF) {
        this->border = border;

        hgeQuad* quad = new hgeQuad;
        quad->v[0].x = cells[i][j].leftTop.x + border;
        quad->v[0].y = cells[i][j].leftTop.y + border;

        quad->v[1].x = cells[i][j].rightTop.x - border;
        quad->v[1].y = cells[i][j].rightTop.y + border;

        quad->v[2].x = cells[i][j].rightBottom.x - border;
        quad->v[2].y = cells[i][j].rightBottom.y - border;

        quad->v[3].x = cells[i][j].leftBottom.x + border;
        quad->v[3].y = cells[i][j].leftBottom.y - border;

        quad->v[0].tx = 0.0f;
        quad->v[0].ty = 0.0f;
        quad->v[1].tx = 1.0f;
        quad->v[1].ty = 0.0f;
        quad->v[2].tx = 1.0f;
        quad->v[2].ty = 1.0f;
        quad->v[3].tx = 0.0f;
        quad->v[3].ty = 1.0f;

        for (int i = 0; i < 4; i++)
        {
            quad->v[i].z = 0.5f;
            quad->v[i].col = color;
        }
        const auto tex = hge->Texture_Create(width_cell / 2, width_cell / 2);
        struct CColor {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };
        CColor* tex_data;
        tex_data = reinterpret_cast<CColor*>(hge->Texture_Lock(tex));
        int pitch = hge->Texture_GetWidth(tex, false);
        memset(tex_data, color, sizeof(CColor) * pitch * hge->Texture_GetHeight(tex, false));
        hge->Texture_Unlock(tex);
        quad->tex = tex;

        quad->blend = BLEND_DEFAULT;

        return quad;
    }

    bool this_is_pawn(const pawns_t& pawns, const size_t x, const size_t y) const {
        for (size_t i = 0; i < pawns.size(); i++) {
            if (pawns[i].x == x && pawns[i].y == y)
                return true;
        }
        return false;
    }

    void update() {
        render();
        if (state_step == STATE_STEP_ENEMY && cycle) {
            findPaths();
            state_step = STATE_STEP_PLAYER;
        } else {
            if (stateCursor.choice) {
                fnt->printf(10, 70, HGETEXT_LEFT, "x=%d y=%d", stateCursor.x, stateCursor.y);
                renderCell(stateCursor.x, stateCursor.y, 0xFFFF00FF);
            }
            updateCursor();
        }
        dtime += hge->Timer_GetDelta();

        fnt->printf(10, 10, HGETEXT_LEFT, "Timer_GetTime: %f", hge->Timer_GetTime());
        //fnt->printf(10, 110, HGETEXT_LEFT, "%s", str.c_str());

    }

    void printState(std::string str) {
        this->str = str;
    }

    void updateCursor()
    {
        float mx, my;
        hge->Input_GetMousePos(&mx, &my);
        if (mx > 0 && mx < width_cell * count_cell_x && my > 0 && my < width_cell * count_cell_y) {
            int index_x, index_y;
            index_x = mx / width_cell;
            index_y = my / width_cell;
            renderCell(index_x, index_y, 0xFF5500FF);


            if (hge->Input_GetKeyState(HGEK_RBUTTON)) {
                int index_x_r, index_y_r;
                index_x_r = mx / width_cell;
                index_y_r = my / width_cell;
                std::vector<std::vector<int>> mapPawns(COUNT_RANGE, std::vector<int>(COUNT_RANGE, 0));
                remapPawns(mapPawns);

                printState(std::to_string(index_x_r) + " " + std::to_string(index_y_r) + " " + std::to_string(mapPawns[index_x_r][index_y_r]) + " " + std::to_string(getPawnPlaceOfNumber(index_x_r*COUNT_RANGE+index_y_r, enemy)));
            }
            if (hge->Input_GetKeyState(HGEK_LBUTTON)) {

                if (!stateCursor.choice) {
                    if (!stateCursor.mousePressed && this_is_pawn(player, index_x, index_y)) {
                        stateCursor.pressedTime = hge->Timer_GetTime();
                        stateCursor.x = index_x;
                        stateCursor.y = index_y;
                        stateCursor.mousePressed = true;
                        printState(std::string("$ " + std::to_string(index_x) + " " + std::to_string(index_y)));
                    }
                    else {
                    }
                }
                else {
                    if (!stateCursor.mousePressed) {
                        stateCursor.pressedTime = hge->Timer_GetTime();
                        stateCursor.mousePressed = true;
                        printState(std::string("$ " + std::to_string(index_x) + " " + std::to_string(index_y)));
                    }
                    else {

                    }
                }
            }
            else {
                if (stateCursor.mousePressed) {
                    stateCursor.mousePressed = false;
                    stateCursor.pressedTime = hge->Timer_GetTime() - stateCursor.pressedTime;
                    if (stateCursor.pressedTime < stateCursor.deltaForDifferent) {
                        if (stateCursor.choice) {
                            if (index_x == stateCursor.x - 1 && index_y == stateCursor.y ||
                                index_x == stateCursor.x + 1 && index_y == stateCursor.y ||
                                index_x == stateCursor.x && index_y == stateCursor.y - 1 ||
                                index_x == stateCursor.x && index_y == stateCursor.y + 1) {
                                printState("***");
                                if (!this_is_pawn(player, index_x, index_y) && !this_is_pawn(enemy, index_x, index_y)) {
                                    move_pawn(stateCursor.x, stateCursor.y, index_x, index_y, player);
                                    state_step = STATE_STEP_ENEMY;
                                }
                            }
                        }
                        stateCursor.choice = !stateCursor.choice;
                    }
                }
                else {
                }
            }
        }
    }

    int move_pawn(const size_t index_x, const  size_t index_y, const  size_t to_x, const  size_t to_y, pawns_t& pawns) {
        for (size_t i = 0; i < pawns.size(); i++) {
            if (pawns[i].x == index_x && pawns[i].y == index_y) {
                pawns[i].x = to_x;
                pawns[i].y = to_y;
                pawns[i].number = to_x * COUNT_RANGE + to_y;
                pawns[i].quad->v[0].x = cells[to_x][to_y].leftTop.x + border;
                pawns[i].quad->v[0].y = cells[to_x][to_y].leftTop.y + border;

                pawns[i].quad->v[1].x = cells[to_x][to_y].rightTop.x - border;
                pawns[i].quad->v[1].y = cells[to_x][to_y].rightTop.y + border;

                pawns[i].quad->v[2].x = cells[to_x][to_y].rightBottom.x - border;
                pawns[i].quad->v[2].y = cells[to_x][to_y].rightBottom.y - border;

                pawns[i].quad->v[3].x = cells[to_x][to_y].leftBottom.x + border;
                pawns[i].quad->v[3].y = cells[to_x][to_y].leftBottom.y - border;
                return i;
            }
        }
        return -1;
    }

    void render() {
        for (int i = 0; i < cells.size(); i++) {
            for (int j = 0; j < cells.size(); j++) {
                renderCell(i, j, 0xFFFFFFFF);
            }
        }
        for (int i = 0; i < enemy.size(); i++) {
            hge->Gfx_RenderQuad(enemy[i].quad);
        }
        for (int i = 0; i < player.size(); i++) {
            hge->Gfx_RenderQuad(player[i].quad);
        }
    }
};

