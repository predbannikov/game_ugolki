
#pragma once





















/*



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

class Game_ {
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

    Game_(float width = 10.0f, int x = 8, int y = 8) : width_cell(width), count_cell_x(x), count_cell_y(y) {
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
        }
        else {
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

                printState(std::to_string(index_x_r) + " " + std::to_string(index_y_r) + " " + std::to_string(mapPawns[index_x_r][index_y_r]) + " " + std::to_string(getPawnPlaceOfNumber(index_x_r * COUNT_RANGE + index_y_r, enemy)));
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

*/

