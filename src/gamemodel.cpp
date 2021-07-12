#include "gamemodel.h"

GameModel::GameModel(const size_t width_side) {
    board = new Board(width_side);
    cell_debug = Cell(-1, -1, -1);
}

/* Создать фигуры
left_top_corner - левая верхняя точка, начиная с которой будут расставляться фигуры
width - количество фигур в ряд
height - количество фигур в колонку*/

PawnsPlayer* GameModel::createPawns(const PlayerPlace* player_place, const int8_t id_player) {
    int32_t color;
    if (players_pawns.empty())
        color = 0xFFBBBBBB;
    else
        color = 0xFF1111FF;
    PawnsPlayer* player_pawns = new PawnsPlayer(player_place, color, id_player);
    players_pawns.push_back(player_pawns);
    return player_pawns;
}

/* Расставить фигуры на доске*/

void GameModel::arrangeFigures(const pawns_t& pawns, const int8_t plyer_side) {
    for (size_t i = 0; i < pawns.size(); i++) {
        board->arr_squars[pawns[i].y][pawns[i].x]->fillType = plyer_side;
    }
}

inline PointAB GameModel::findPaths(pawns_t& pawns) {

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
                    int length = -1;
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

    //int ret = move_pawn(x, y, x_to, y_to, pawns);
    if (*it == 55)
        std::cout << "";
    //if (paths.begin()->first < 1.1f) {
    //    pawns[ret].place = true;
    //}


    return PointAB();
}

/* Установить размер спрайта для пешки*/

void GameModel::setSizePawn(const float& w, const float& h) {
    for (size_t p = 0; p < players_pawns.size(); p++)
        for (size_t i = 0; i < players_pawns[p]->pawns.size(); i++) {
            players_pawns[p]->pawns[i].spr_pawn->SetTextureRect(0, 0, w, h);
        }
}

void GameModel::moveCursore() {
    hge->Input_GetMousePos(&stateMouse.mx, &stateMouse.my);
    notifyUpdate();
}

void GameModel::printString(std::string str, float x, float y) {
    message.x = x;
    message.y = y;
    message.str = str;
    notifyUpdate();
}

inline void GameModel::selectCell() {
    notifyUpdate();
}

// TODO На данный момент всё завязано статически, ищутся ячейки ближайшие к правому нижнему углу
/* Получить номера ячеек к которым искать пути и положить их в вектор*/

void GameModel::getCellsToSearchPath(std::vector<int>& searchCells, size_t barrier_tolerance) {
    for (int i = board->arr_squars.size() - 1; i >= board->arr_squars.size() - barrier_tolerance; i--) {
        for (int j = board->arr_squars.size() - 1; j >= board->arr_squars.size() - barrier_tolerance; j--) {
            if (board->arr_squars[i][j]->fillType == 0)
                searchCells.push_back(i * board->arr_squars.size() + j);
        }
    }
}

void GameModel::getCellsTarget(cells_t& targetCells, const PawnsPlayer& player, size_t barrier_tolerance, bool inside) {
    targetCells.clear();
    size_t width = player.enemyPlace->width_board;
    size_t height = player.enemyPlace->height_board;
    Point t = player.enemyPlace->most_interest_point;

    std::vector<std::vector<size_t> > map(height, std::vector<size_t>(width, 0));
    std::stack<std::pair<int, int>>stackCells;
    stackCells.push({ t.y, t.x });
    while (!stackCells.empty()) {
        auto [y, x] = stackCells.top();
        stackCells.pop();
        map[y][x] = 1;
        if (x < width - 1 && map[y][x + 1] == 0 && abs(x + 1 - t.x) < barrier_tolerance) {
            stackCells.push({ y, x + 1 });
        }
        if (x > 0 && map[y][x - 1] == 0 && abs(x - 1 - t.x) < barrier_tolerance) {
            stackCells.push({ y, x - 1 });
        }
        if (y < height - 1 && map[y + 1][x] == 0 && abs(y + 1 - t.y) < barrier_tolerance) {
            stackCells.push({ y + 1, x });
        }
        if (y > 0 && map[y - 1][x] == 0 && abs(y - 1 - t.y) < barrier_tolerance) {
            stackCells.push({ y - 1, x });
        }
    }

    if (inside) {
        const cells_t& cells = player.enemyPlace->cells;
        for (size_t i = 0; i < cells.size(); i++) {
            if (map[cells[i].y][cells[i].x] == 1 && !board->arr_squars[cells[i].y][cells[i].x]->fillType) {
                targetCells.push_back(Cell(cells[i].x, cells[i].y, cells[i].n));
            }
        }
    }
    else {
        for (size_t i = 0; i < height; i++) {
            for (size_t j = 0; j < width; j++) {
                if (map[i][j] == 1 && !board->arr_squars[i][j]->fillType) {
                    targetCells.push_back(Cell(j, i, i * height + j));
                }
            }
        }
    }
}

void GameModel::shortWay(const pawns_t& pawns, int from, int to, std::vector<size_t>& path, int& length) {
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
        //fnt->printf(170, 10, HGETEXT_LEFT, "no path from %i to %i", v, t);
    }
    else {
        length = d[t];
        for (int cur = t; cur != -1; cur = p[cur]) {
            path.push_back(cur);
        }
        std::reverse(path.begin(), path.end());
    }
}

/* Передвинуть пешку*/

inline int GameModel::move_pawn(const size_t index_x, const size_t index_y, const size_t to_x, const size_t to_y, PawnsPlayer& player) {
    for (size_t i = 0; i < player.pawns.size(); i++) {
        if (player.pawns[i].x == index_x && player.pawns[i].y == index_y) {
            player.pawns[i].x = to_x;
            player.pawns[i].y = to_y;
            player.pawns[i].n = to_y * board->arr_squars.size() + to_x;
            board->arr_squars[index_y][index_x]->fillType = 0;
            board->arr_squars[to_y][to_x]->fillType = player.side;
            return i;
        }
    }
    return -1;
}


///* узнать есть ли пешка на клетке из множества pawns*/

inline bool GameModel::this_is_pawn(const pawns_t& pawns, const size_t x, const size_t y) const {
    for (size_t i = 0; i < pawns.size(); i++) {
        if (pawns[i].x == x && pawns[i].y == y)
            return true;
    }
    return false;
}

bool GameModel::clickCursor() {
    bool choice = false;
    float mx, my;
    hge->Input_GetMousePos(&mx, &my);
    if (mx > 0 && mx < board->width_item_cell * board->count_cells_row && my > 0 && my < board->width_item_cell * board->count_cells_col) {
        int index_x, index_y;
        index_x = mx / board->width_item_cell;
        index_y = my / board->width_item_cell;

        if (hge->Input_GetKeyState(HGEK_RBUTTON)) {
            int index_x_r, index_y_r;
            index_x_r = mx / board->width_item_cell;
            index_y_r = my / board->width_item_cell;
            stateMouse.rightButton = true;
        }
        else {
            stateMouse.rightButton = false;
        }
        if (hge->Input_GetKeyState(HGEK_LBUTTON)) {
            if (!stateMouse.choice) {
                if (!stateMouse.mousePressed ) {
                    stateMouse.pressedTime = hge->Timer_GetTime();
                    stateMouse.x = index_x;
                    stateMouse.y = index_y;
                    stateMouse.mousePressed = true;
                    cells_target.push_back(Cell(index_x, index_y, board->count_cells_col * index_y + index_x));
                }
                else {
                    cells_target.clear();
                }
            }
            else {
                if (!stateMouse.mousePressed) {
                    stateMouse.pressedTime = hge->Timer_GetTime();
                    stateMouse.mousePressed = true;
                }
                else {

                }
            }
        }
        else {
            if (stateMouse.mousePressed) {
                stateMouse.mousePressed = false;
                stateMouse.pressedTime = hge->Timer_GetTime() - stateMouse.pressedTime;
                if (stateMouse.pressedTime < stateMouse.deltaForDifferent) {
                    if (stateMouse.choice) {
                        choice = true;
                    }
                    stateMouse.choice = !stateMouse.choice;
                }
            }
            else {
                if (stateMouse.choice) {
                    cells_target.clear();
                    for (size_t i = stateMouse.y; i <= index_y; i++) {
                        for (size_t j = stateMouse.x; j <= index_x; j++) {
                            cells_target.push_back(Cell(j, i, board->count_cells_col * i + j));
                        }
                    }                
                }

            }
        }
    }
    notifyUpdate();
    return choice;
}

bool GameModel::clickCursor(PawnsPlayer& player)
{
    bool choice = false;
    float mx, my;
    hge->Input_GetMousePos(&mx, &my);
    if (mx > 0 && mx < board->width_item_cell * board->count_cells_row && my > 0 && my < board->width_item_cell * board->count_cells_col) {
        int index_x, index_y;
        index_x = mx / board->width_item_cell;
        index_y = my / board->width_item_cell;

        if (hge->Input_GetKeyState(HGEK_RBUTTON)) {
            int index_x_r, index_y_r;
            index_x_r = mx / board->width_item_cell;
            index_y_r = my / board->width_item_cell;
            stateMouse.rightButton = true;
        }
        else {
            stateMouse.rightButton = false;
        }
        if (hge->Input_GetKeyState(HGEK_LBUTTON)) {
            if (!stateMouse.choice) {
                if (!stateMouse.mousePressed && this_is_pawn(player.pawns, index_x, index_y)) {
                    stateMouse.pressedTime = hge->Timer_GetTime();
                    stateMouse.x = index_x;
                    stateMouse.y = index_y;
                    stateMouse.mousePressed = true;
                }
                else {
                }
            }
            else {
                if (!stateMouse.mousePressed) {
                    stateMouse.pressedTime = hge->Timer_GetTime();
                    stateMouse.mousePressed = true;
                }
                else {

                }
            }
        }
        else {
            if (stateMouse.mousePressed) {
                stateMouse.mousePressed = false;
                stateMouse.pressedTime = hge->Timer_GetTime() - stateMouse.pressedTime;
                if (stateMouse.pressedTime < stateMouse.deltaForDifferent) {
                    if (stateMouse.choice) {
                        if (index_x == stateMouse.x - 1 && index_y == stateMouse.y ||
                            index_x == stateMouse.x + 1 && index_y == stateMouse.y ||
                            index_x == stateMouse.x && index_y == stateMouse.y - 1 ||
                            index_x == stateMouse.x && index_y == stateMouse.y + 1) {
                            if (!board->arr_squars[index_y][index_x]->fillType) {
                                move_pawn(stateMouse.x, stateMouse.y, index_x, index_y, player);
                                choice = true;
                            }
                        }
                    }
                    stateMouse.choice = !stateMouse.choice;
                }
            }
            else {
            }
        }
    }
    notifyUpdate();
    return choice;
}

inline bool GameModel::getPawnPlaceOfNumber(const size_t number_pawn, const pawns_t& pawns) {
    size_t index_x, index_y;
    getIndexs(number_pawn, index_x, index_y);
    for (size_t i = 0; i < pawns.size(); i++) {
        if (pawns[i].x == index_x && pawns[i].y == index_y) {
            return pawns[i].place;
        }
    }
    return false;
}

/* Провека на местах ли пешки */
bool GameModel::checkWin(const PawnsPlayer& player) {
    bool check = true;
    for (size_t i = 0; i < player.pawns.size(); i++) {
        bool checkInside = false;
        for (auto& cell : player.enemyPlace->cells) {
            if (player.pawns[i].n == cell.n) {
                checkInside = true;;
            }
        }
        if (!checkInside)
            check = false;
    }
    return check;
}

inline bool GameModel::checkEdge(Edge ed, std::vector<Edge>& vec_edge) {
    for (size_t i = 0; i < vec_edge.size(); i++)
        if (ed.a == vec_edge[i].a && ed.b == vec_edge[i].b)
            return true;
    return false;
}

inline void GameModel::getIndexs(const size_t numberCell, size_t& x, size_t& y) {
    x = numberCell % board->arr_squars.size();
    y = numberCell / board->arr_squars.size();
}

inline void GameModel::calcVecEdge(const pawns_t& pawns, std::vector<Edge>& edge) {
    for (int i = 0; i < board->arr_squars.size(); i++) {
        for (int j = 0; j < board->arr_squars.size(); j++) {
            Edge ed;
            ed.cost = 1;
            ed.a = board->arr_squars[i][j]->n;

            if (getPawnPlaceOfNumber(ed.a, pawns))
                continue;
            if (j - 1 >= 0 && !board->arr_squars[i][j - 1]->fillType) {
                ed.b = j - 1 + i * board->arr_squars.size();
                edge.push_back(ed);
            }
            if (j + 1 < board->arr_squars.size() && !board->arr_squars[i][j + 1]->fillType) {
                ed.b = i * board->arr_squars.size() + j + 1;
                edge.push_back(ed);
            }
            if (i - 1 >= 0 && !board->arr_squars[i - 1][j]->fillType) {
                ed.b = (i - 1) * board->arr_squars.size() + j;
                edge.push_back(ed);
            }
            if (i + 1 < board->arr_squars.size() && !board->arr_squars[i + 1][j]->fillType) {
                ed.b = (i + 1) * board->arr_squars.size() + j;
                edge.push_back(ed);
            }
        }
    }

}
