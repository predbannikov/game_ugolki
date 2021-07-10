#include "controller.h"

Controller::Controller(GameModel* model_) : model(model_)
{
    player_pawns = model->createPawns(new PlayerPlace(model->board, Point(0, 0), 3, 3));   // Создать фигуры 
    model->arrangeFigures(player_pawns->pawns);             // Расставить фигуры на доске
    model->setSizePawn(WIDTH_PAWN, WIDTH_PAWN);
    const Point& p = player_pawns->enemyPlace->most_interest_point;
    width = player_pawns->enemyPlace->width_board;
    height = player_pawns->enemyPlace->height_board;

    model->cell_debug = Cell(p.x, p.y, p.x + p.y * player_pawns->enemyPlace->width_board);

}

inline bool Controller::processGame() {
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::map<double, std::vector<size_t>> paths;

    pawns_t& pawns = player_pawns->pawns;

    size_t barrier_tolerance = 0;
    std::vector<int> numbersCellsWins;
    while (paths.empty()) {
        if (barrier_tolerance != 0)
            std::cout << "";
        numbersCellsWins.clear();
        if (barrier_tolerance == COUNT_CELLS_ROW) {
            std::cout << "pad";
            return false;
        }
        model->getCellsToSearchPath(numbersCellsWins, barrier_tolerance);    // Получаем ячейки для поиска
        for (size_t i = 0; i < pawns.size(); i++) {
            if (!pawns[i].place)
                for (size_t j = 0; j < numbersCellsWins.size(); j++) {
                    std::vector<size_t> path;
                    int length = -1;
                    model->shortWay(pawns, pawns[i].n, numbersCellsWins[j], path, length);
                    if (length > 0)
                        paths[length] = path;
                }
        }
        barrier_tolerance++;
        if (!paths.empty()) {
            std::cout << "";
            if (barrier_tolerance < model->barrier.currentLvlBarrier) {
                for (size_t i = 0; i < pawns.size(); i++)
                    pawns[i].place = false;
                paths.clear();
                barrier_tolerance = 0;
                model->barrier.currentLvlBarrier = barrier_tolerance;
            }
        }
    }

    model->barrier.currentLvlBarrier = barrier_tolerance;

    if (paths.size() == 1)
        std::cout << "";

    auto it = paths.begin()->second.begin();

    int ret = model->move_pawn(*it % COUNT_CELLS_ROW, *it / COUNT_CELLS_ROW, *(it + 1) % COUNT_CELLS_ROW, *(it + 1) / COUNT_CELLS_ROW, pawns);
    if (*it == 55)
        std::cout << "";
    if (paths.begin()->first == 1) {
        pawns[ret].place = true;        // Когда длина для последнего перемещения пешки была равна 1 значит пешка добралась до места назначения, делаем пометку
    }
    return true;
}

inline void Controller::getPaths(std::map<size_t, std::vector<size_t>>& paths, const cells_t& target_cells) {
    for (size_t i = 0; i < player_pawns->pawns.size(); i++) {
        if (!player_pawns->pawns[i].place)
            for (size_t j = 0; j < target_cells.size(); j++) {
                std::vector<size_t> path;
                int length = -1;
                model->shortWay(player_pawns->pawns, player_pawns->pawns[i].n, target_cells[j].n, path, length);
                if (length > 0)
                    paths[length] = path;
            }
    }

}

inline void Controller::process() {
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::map<size_t, std::vector<size_t>> paths;
    size_t barrier_tolerance = 0;
    cells_t targetCells;
    model->getCellsTarget(targetCells, *player_pawns->enemyPlace, barrier_tolerance, true);
    getPaths(paths, targetCells);

    std::vector<size_t> path = paths.begin()->second;
    model->cells_debug.clear();
    for (size_t i = 0; i < path.size(); i++) {
        model->cells_debug.push_back(Cell(path[i] % width, path[i] / width, path[i]));
    }
}

 void Controller::perform()
{
    //model->findPaths(player_pawns->pawns);

    int barrier = 2;
    model->getCellsTarget(model->cells_target, *player_pawns->enemyPlace, barrier, true);

    process();
    //processGame();
    std::string points_str = player_pawns->playerPlace->str;
    model->printString(points_str, 450, 350);




    std::string str = points_str;
    //model->printString(str);
    if (temp == 1)
        std::cout << "stop";

    if (hge->Input_IsMouseOver()) {
        model->updateCursor();
    }
    temp++;
}
