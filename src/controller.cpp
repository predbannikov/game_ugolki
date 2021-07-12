#include "controller.h"

int8_t Controller::id_player;

Controller::Controller(GameModel* model_, bool ai, Point startPoint, size_t width_place_pawn, size_t height_place_pawn) : model(model_), AI(ai)
{
    id_player++;
    player_pawns = model->createPawns(new PlayerPlace(model->board, startPoint, width_place_pawn, height_place_pawn), id_player);   // Создать фигуры 
    model->arrangeFigures(player_pawns->pawns, id_player);              // Расставляем фигуры на доске
    model->setSizePawn(WIDTH_PAWN, WIDTH_PAWN);                         // ширина длина пешки
    width = player_pawns->enemyPlace->width_board;                      // TODO не нужны удалить окуратно 
    height = player_pawns->enemyPlace->height_board;


    const Point& p = player_pawns->enemyPlace->most_interest_point;     // Наиболее интересная точка 
    if(ai)
        model->cell_debug = Cell(p.x, p.y, p.x + p.y * player_pawns->enemyPlace->width_board);      // Метка куда будет нацелен AI

    // Создаём копию расположения игрока в памяти
    memory.resize(model->board->count_cells_row);
    for (size_t i = 0; i < memory.size(); i++) {
        memory[i].resize(model->board->count_cells_col);
        for (size_t j = 0; j < memory.front().size(); j++) {
            memory[i][j] = new Square(*model->board->arr_squars[i][j]);
        }
    }
}


inline void Controller::getPaths(std::map<size_t, std::vector<size_t>>& paths, const cells_t& target_cells) {
    for (size_t i = 0; i < player_pawns->pawns.size(); i++) {
        if (!player_pawns->pawns[i].place)                      // Если задача не выполнена 
            for (size_t j = 0; j < target_cells.size(); j++) {
                std::vector<size_t> path;
                int length = -1;
                model->shortWay(player_pawns->pawns, player_pawns->pawns[i].n, target_cells[j].n, path, length);
                if (length > 0)
                    paths[length] = path;
            }
    }

}

inline bool Controller::nextSet(int* a, int n, int m)
{
    int j = m - 2;
    while (j >= 0 && a[j] == n)
        j--;
    if (j < 0)
        return false;
    if (a[j] >= n)
        j--;
    a[j]++;
    if (j == m - 1)
        return true;
    for (int k = j + 1; k < m; k++)
        a[k] = 1;
    return true;
}

bool Controller::upperStage(size_t& stage)
{
    stage++;
    if (stage > range)
        return false;
    return true;
}

int Controller::movePawnAB(size_t a, size_t b)
{
    return model->move_pawn(a % COUNT_CELLS_ROW, a / COUNT_CELLS_ROW, b % COUNT_CELLS_ROW, b / COUNT_CELLS_ROW, *player_pawns);
}

bool Controller::perform(bool &win)
{
    bool clickState = false;
    if (!AI) {
        if (model->clickCursor(*player_pawns))
            clickState = true;
        if (model->stateMouse.rightButton) {
            clickState = true;
        }
    }
    else {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));

        /* Тут будем хратиться найденные пути в отсортированном порядке по длине, и обновляться после каждой вхождения в цикл */
        std::map<size_t, std::vector<size_t>> paths;
        std::map<size_t, std::vector<size_t>> tmpPaths;
        int smth_digit = 0;
        int n, m, * a;
        n = 3;  // Набор
        m = 3;  // Количество
        int h = n > m ? n : m; // размер массива а выбирается как max(n,m)
        a = new int[h];
        for (size_t i = 0; i < h; i++)
            a[i] = 1;
        std::string str;
        while (nextSet(a, n, m)) {
            for (int i = 0; i < m; i++) {
                // Можно попробовать поиграться комбинаторным поиском путей из разного сочетания алгоритмов
                combinatAlgo(tmpPaths, a);
                paths.merge(tmpPaths);
                tmpPaths.clear();
            }
        }
        delete [] a;
        
        int dist = 0;
        if (!paths.empty()) {
            auto itEnd = paths.end();
            itEnd--;
            auto itStart = paths.begin();
            for (; itStart != itEnd; itStart++) {
                itStart->second;
                dist = player_pawns->distanceToMIP(*itStart->second.begin(), *(itStart->second.begin() + 1));
                if (dist <= 0)
                    break;
            }
            auto it = itStart->second.begin();
            int ret = movePawnAB(*it, *(it+1));
            if (*it == 55)
                std::cout << "";
            if (paths.begin()->first == 1)
                player_pawns->pawns[ret].place = true;
            if (model->checkWin(*player_pawns)) {
                model->printString("WIN");
                win = true;
            }
        }
        clickState = true;
    }

    //model->printString(str);
    if (temp == 1)
        std::cout << "stop";


    temp++;
    return clickState;
}

void Controller::combinatAlgo(std::map<size_t, std::vector<size_t>>& paths, int *sequence)
{
    bool rationalStep = true;
    int barrier = 0;    // Начинаем с допускаемым отступом в 0 шагов
    cells_t cellsTarget;

    /* Приоритет поиска маршрута, ищем сначала внутри области PawnPlayer.enemyPlace (Область где расположены пешки врага)
    * когда внутри этой области отсутствуют пешки, сбрасываем барьер на 0 и ищем маршрут по направлению включая область за пределами PawnPlayer.enemyPlace
    * так нужно для случая когда точки врага изначально располагаются не в квадрате а в прямоугольнике */
    bool inside = true;

    size_t first_algo = 1;
    size_t second_algo = 2;
    size_t third_algo = 3;
    size_t fourth_algo = 4;
    size_t stage = 0;



    /* Если у нас нет доступных маршрутов продолжаем цикл поиска */
    while (paths.empty()) {

        int dist = 0;   // Дистанция если положительная значит после хода мы удалились от главной цели

        switch (*(sequence + stage))
        {
        case 1:
            inside = true;
            /* Получаем ячейки которые не заняты по отношению к точке MIP (наиболее интересующей точке, она вычисляется на этапе расстановки пешек)
            barrier - указывает допускаемый отступ от самой точки (во все стороны)
            inside = 1 - поиск только в области для победы
            inside = 0 - поиск только по отношению к точке без привязки к области для победы*/
            model->getCellsTarget(cellsTarget, *player_pawns, barrier, inside);
            break;
        case 2:
            inside = false;
            /* Получаем ячейки которые не заняты по отношению к точке MIP (наиболее интересующей точке, она вычисляется на этапе расстановки пешек)
            barrier - указывает допускаемый отступ от самой точки (во все стороны)
            inside = 1 - поиск только в области для победы
            inside = 0 - поиск только по отношению к точке без привязки к области для победы*/

            model->getCellsTarget(cellsTarget, *player_pawns, barrier, inside);
            break;
        case 3:
            if (!checkMem()) {
                player_pawns->resetPawns();
                saveMem();
                if (upperStage(stage))
                    break;
                continue;
            }

            break;
        case 4:
            /* обнаружение обратного хода - бесполезный ход, можно поискать маршруты получше, */
            if (!paths.empty()) {
                do {
                    auto it = paths.begin()->second.begin();
                    dist = player_pawns->distanceToMIP(*it, *(it + 1));
                    if (dist > 0) {
                        paths.erase(paths.begin());
                    }
                } while (!paths.empty() && dist > 0);
                if (upperStage(stage))
                    break;
            }

            break;
        default:
            break;
        }


        /* Получаем доступные пути к интересующим точкам*/
        getPaths(paths, cellsTarget);

        barrier++;
        if (barrier > player_pawns->enemyPlace->height_place && barrier > player_pawns->enemyPlace->width_place && inside) {
            barrier = 0;
            if (upperStage(stage))
                break;
        }



        // Если барьер стал больше ширины доски, значит нет доступных ходов, выходим из поиска
        if (barrier > player_pawns->playerPlace->width_board) {
            player_pawns->resetPawns();
            saveMem();

            if (upperStage(stage))
                break;
        }
    }
}

inline bool Controller::checkMem() {
    bool check = true;
    for (auto &cell : player_pawns->enemyPlace->cells) {
        int8_t type = model->board->arr_squars[cell.y][cell.x]->fillType;
        if (player_pawns->side != type) {
            if (memory[cell.y][cell.x]->fillType != type) {
                check = false;
            }
        }
    }
    return check;
}

/* Сохраним в памяти состояние области врага 
сохранить нужно именно только пешки врага, запоминание своих пешек будут инициализировать ложные срабатывания*/
inline void Controller::saveMem() {
    for (auto &cell : player_pawns->enemyPlace->cells) {
        int8_t type = model->board->arr_squars[cell.y][cell.x]->fillType;
        if (player_pawns->side != type) {
            memory[cell.y][cell.x]->fillType = type;
        }
    }
}
