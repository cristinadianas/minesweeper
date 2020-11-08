#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ncursesw/ncurses.h>
using namespace std;

#define ymax 16
#define xmax 30

unsigned int board_rows = 0, board_cols = 0, total_mines;
WINDOW *winindex[ymax][xmax];

#define cell_width 5
#define cell_height 3
#define counter_width 15
#define counter_height 3
#define timer_width 15
#define timer_height 3
#define status_width 30
#define status_height 9

#define color_default 0
#define color_markedcell 1
#define green_on_black 2
#define black_on_black 3
#define red_on_black 4

void bsetup()
{
    int dif;
    cout<<"\n DIFFICULTY: \n 1 = Beginner (9x9 - 10 Mines)  \n 2 = Medium (16x16 - 40 Mines) \n 3 = Difficult (30x16 - 99 Mines) \n 4 = Custom (max: 30x16 - 480 Mines) \n \n";
    cin>>dif;
    while (dif < 1 || dif > 4)
    {
        cout<<"Please enter a valid level of difficulty."<<endl;
        cin>>dif;
    }
    switch(dif) {
        case 1:
            board_cols=9;
            board_rows=9;
            total_mines=10;
            break;
        case 2:
            board_cols=16;
            board_rows=16;
            total_mines=40;
            break;
        case 3:
            board_cols=30;
            board_rows=16;
            total_mines=99;
            break;
        case 4:
            cout<<endl;
            do {
                cout<<"WIDTH = ";
                cin >> board_cols;
            } while (board_cols < 1 || board_cols > xmax);

            do {
                cout<<"HEIGHT = ";
                cin >> board_rows;
            } while (board_rows < 1 || board_rows > ymax);

            do {
                cout<<"MINES = ";
                cin >> total_mines;
            } while (total_mines < 0 || total_mines > board_rows * board_cols);
            break;
    }

}
bool valid_cell(int row, int col)
{
    if (row >= 0 && row < board_rows && col >= 0 && col < board_cols)
        return TRUE;
    return FALSE;
}
void num_neighbours (int row, int col, int v[][xmax])
{
    if (v[row-1][col-1]!=-1) v[row-1][col-1]++;
    if (v[row-1][col]!=-1) v[row-1][col]++;
    if (v[row-1][col+1]!=-1) v[row-1][col+1]++;
    if (v[row+1][col+1]!=-1) v[row+1][col+1]++;
    if (v[row+1][col]!=-1) v[row+1][col]++;
    if (v[row+1][col-1]!=-1) v[row+1][col-1]++;
    if (v[row][col-1]!=-1) v[row][col-1]++;
    if (v[row][col+1]!=-1) v[row][col+1]++;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);		// 0, 0 gives default characters for the vertical and horizontal lines
    return local_win;
}
void show_in_win(int row, int col, char ch, int color_pair)
{
    wbkgd(winindex[row][col], COLOR_PAIR(color_pair));
    mvwaddch(winindex[row][col], cell_height / 2, cell_width / 2, ch);
    wrefresh(winindex[row][col]);
}
void move_cursor (int row, int col)
{
    mvwaddch(winindex[row][col], cell_height / 2, cell_width / 2 - 1, '[');
    mvwaddch(winindex[row][col], cell_height / 2, cell_width / 2 + 1, ']');
    wrefresh(winindex[row][col]);
}
void UDRL (int &row, int &col, int ch)
{
    switch (ch) {
        case KEY_LEFT:
            if (col > 0)
            {
                mvwaddch(winindex[row][col], cell_height / 2, cell_width / 2 - 1, ' ');
                mvwaddch(winindex[row][col], cell_height / 2, cell_width / 2 + 1, ' ');
                wrefresh(winindex[row][col]);
                move_cursor(row, --col);
            }
            break;
        case KEY_RIGHT:
            if (col < board_cols - 1)
            {
                mvwaddch(winindex[row][col], cell_height / 2, cell_width / 2 - 1, ' ');
                mvwaddch(winindex[row][col], cell_height / 2, cell_width / 2 + 1, ' ');
                wrefresh(winindex[row][col]);
                move_cursor(row, ++col);
            }
            break;
        case KEY_UP:
            if (row > 0)
            {
                mvwaddch(winindex[row][col], cell_height / 2, cell_width / 2 - 1, ' ');
                mvwaddch(winindex[row][col], cell_height / 2, cell_width / 2 + 1, ' ');
                wrefresh(winindex[row][col]);
                move_cursor(--row, col);
            }
            break;
        case KEY_DOWN:
            if (row < board_rows - 1)
            {
                mvwaddch(winindex[row][col], cell_height / 2, cell_width / 2 - 1, ' ');
                mvwaddch(winindex[row][col], cell_height / 2, cell_width / 2 + 1, ' ');
                wrefresh(winindex[row][col]);
                move_cursor(++row, col);
            }
            break;
        }
}
void sleep(float seconds)
{
    clock_t startClock = clock();
    float secondsAhead = seconds * CLOCKS_PER_SEC;
    // do nothing until the elapsed time has passed
    while (clock() < startClock + secondsAhead);
    return;
}

int num_markedneighb(int row, int col, bool marked[][xmax])
{
    int neighbours = 0;
    for (int y = row - 1; y <= row + 1; y++)
        for (int x = col - 1; x <= col + 1; x++)
            if (marked[y][x] == TRUE && valid_cell(y, x) == TRUE)
                neighbours++;
    return neighbours;
}
void ZERO_PRESSED (int row, int col, int board_mines[][xmax], bool stepped[][xmax], bool marked[][xmax], int &steps)
{
    steps++;
    stepped[row][col] = TRUE;
    werase(winindex[row][col]);
    wrefresh(winindex[row][col]);
    for (int y = row - 1; y <= row + 1; y++)
        for (int x = col - 1; x <= col + 1; x++)
            if (valid_cell(y, x) == TRUE && stepped[y][x] == FALSE && marked[y][x] == FALSE)
                if (board_mines[y][x] == 0)
                    ZERO_PRESSED(y, x, board_mines, stepped, marked, steps);
                else
                {
                    steps++;
                    stepped[y][x] = TRUE;
                    show_in_win(y, x, '0' + board_mines[y][x], green_on_black);
                }
}
void NEWCELL_PRESSED (int row, int col, int board_mines[][xmax], bool stepped[][xmax], bool marked[][xmax], bool &mine_pressed, int &steps)
{
    switch (board_mines[row][col])
    {
        case 0:
            ZERO_PRESSED(row, col, board_mines, stepped, marked, steps);
            break;
        case -1:
            stepped[row][col] = TRUE;
            mine_pressed = TRUE;
            break;
        default:
            steps++;
            stepped[row][col] = TRUE;
            show_in_win(row, col, '0' + board_mines[row][col], green_on_black);
            break;
    }
}

int main()
{
    int board_mines[ymax][xmax] = {0}, y, x, laid_mines = 0;
    if (board_rows == 0)
        bsetup();
    srand (time(NULL));

    for (y=0; y < board_rows; y++)
        for (x=0; x < board_cols; x++)
            if (total_mines - laid_mines != 0 && rand() < (RAND_MAX * (total_mines - laid_mines) / (board_rows * board_cols - (y * board_cols + x))))
            {
                board_mines[y][x]=-1;
                laid_mines++;
                num_neighbours(y, x, board_mines);
            }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    refresh();

    for (y=0; y < board_rows; y++)
        for (x=0; x < board_cols; x++)
        {
            winindex[y][x] = create_newwin(cell_height, cell_width, y*cell_height, x*cell_width);
            wrefresh(winindex[y][x]);
        }

    start_color();
    short COLOR_MARKEDCELL = COLOR_YELLOW;
    if (can_change_color() == TRUE)
    {
        COLOR_MARKEDCELL = 8;
        init_color(COLOR_MARKEDCELL, 1000, 369, 55); //Orange RGB: 255, 94, 14
    }
    init_pair(color_markedcell, COLOR_MARKEDCELL, COLOR_BLACK);
    init_pair(green_on_black, COLOR_GREEN, COLOR_BLACK);
    init_pair(black_on_black, COLOR_BLACK, COLOR_BLACK);
    init_pair(red_on_black, COLOR_RED, COLOR_BLACK);

    int marked_cells = 0, steps = 0;
    bool stepped[ymax][xmax] = {0}, marked[ymax][xmax] = {0}, mine_pressed = FALSE;

    WINDOW *counter = create_newwin(counter_height, counter_width, 0, board_cols * cell_width);
    mvwprintw(counter, 1, 2, "%d/%d Mines ", marked_cells, total_mines);
    wrefresh(counter);

    WINDOW *status = create_newwin(status_height, status_width, counter_height, board_cols * cell_width);
    mvwaddstr(status, 4, 7, "You've got this!");
    wrefresh(status);

    clock_t startClock = clock();
    WINDOW *timer = create_newwin(timer_height, timer_width, 0, board_cols * cell_width + counter_width);
    mvwprintw(timer, 1, 2, "%d seconds", (clock() - startClock)/CLOCKS_PER_SEC);
    wrefresh(timer);

    move_cursor(0, 0); x = 0; y = 0;

    int ch;
    while (mine_pressed == FALSE && steps != (board_rows * board_cols) - total_mines)
    {
        while ((ch = getch()) != 'q' && ch != ' ' && ch != 'x')
        {
            if (ch != ERR)
                UDRL(y, x, ch);
            mvwprintw(timer, 1, 2, "%d seconds", (clock() - startClock)/CLOCKS_PER_SEC);
            wrefresh(timer);
        }
        switch (ch) {
            case ' ':
                if (marked[y][x] == FALSE && stepped[y][x] == FALSE)
                    NEWCELL_PRESSED(y, x, board_mines, stepped, marked, mine_pressed, steps);
                else if (stepped[y][x] == TRUE  &&  num_markedneighb(y, x, marked) == board_mines[y][x])
                    for (int i = y-1; i <= y+1; i++)
                        for (int j = x-1; j <= x+1; j++)
                            if (marked[i][j] == FALSE && stepped[i][j] == FALSE && valid_cell(i, j) == TRUE)
                                NEWCELL_PRESSED(i, j, board_mines, stepped, marked, mine_pressed, steps);
                break;
            case 'x':
                if (stepped[y][x] == FALSE)
                    if (marked[y][x] == FALSE) {
                        mvwprintw(counter, 1, 2, "%d/%d Mines ", ++marked_cells, total_mines);
                        if (marked_cells - 1 == total_mines) {
                            wbkgd(counter, COLOR_PAIR(red_on_black));
                            beep();
                        }
                        wrefresh(counter);
                        show_in_win(y, x, 'x', color_markedcell);
                        marked[y][x] = TRUE;
                    }
                    else {
                        mvwprintw(counter, 1, 2, "%d/%d Mines ", --marked_cells, total_mines);
                        if (marked_cells == total_mines)
                            wbkgd(counter, COLOR_PAIR(color_default));
                        wrefresh(counter);
                        show_in_win(y, x, ' ', color_default);
                        marked[y][x] = FALSE;
                    }
                break;
            case 'q':
                endwin();
                return 0;
        }
        move_cursor(y, x);
    }

    mvwaddstr(status, 4, 1, "                            ");
    mvwaddstr(status, 5, 3, "Press any key besides 'q'");
    mvwaddstr(status, 6, 9, "to continue.");
    wrefresh(status);

    mvwaddch(winindex[y][x], cell_height / 2, cell_width / 2 - 1, ' ');
    mvwaddch(winindex[y][x], cell_height / 2, cell_width / 2 + 1, ' ');
    wrefresh(winindex[y][x]);

    if (steps == (board_rows * board_cols) - total_mines) //WIN
    {
        mvwaddstr(status, 4, 11, "YOU WON");
        wbkgd(status, COLOR_PAIR(green_on_black));
        wrefresh(status);
        for (int i = 0; i < board_rows ; i++)
            for (int j = 0; j < board_cols; j++)
                if (marked[i][j] == FALSE && board_mines[i][j] == -1)
                    show_in_win(i, j, 'x', color_markedcell);
        while ((ch = getch()) == ERR);
    }

    else //LOSE
    {
        flash();
        mvwaddstr(status, 4, 10, "YOU LOST");
        wbkgd(status, COLOR_PAIR(red_on_black));
        wrefresh(status);

        for (int i = 0; i < board_rows ; i++)
            for (int j = 0; j < board_cols; j++)
                if (marked[i][j] == TRUE && board_mines[i][j] != -1)
                    show_in_win(i, j, '-', color_markedcell);
                else if (marked[i][j] == FALSE && board_mines[i][j] == -1)
                    show_in_win(i, j, '*', red_on_black);

        nodelay(stdscr, TRUE);
        while ((ch = getch()) == ERR)
        {
            for (int i = y-1; i <= y+1; i++)
                for (int j = x-1; j <= x+1; j++)
                    if(valid_cell(i, j) == TRUE && board_mines[i][j] == -1 && stepped[i][j] == TRUE)
                    {
                        wbkgd(winindex[i][j], COLOR_PAIR(black_on_black));
                        wrefresh(winindex[i][j]);
                    }
            sleep(0.1);
            for (int i = y-1; i <= y+1; i++)
                for (int j = x-1; j <= x+1; j++)
                    if(valid_cell(i, j) == TRUE && board_mines[i][j] == -1 && stepped[i][j] == TRUE)
                    {
                        wbkgd(winindex[i][j], COLOR_PAIR(red_on_black));
                        wrefresh(winindex[i][j]);
                    }
            sleep(0.1);
        }
    }

    if (ch == 'q')
    {
        endwin();
        return 0;
    }

    nodelay(stdscr, FALSE);
    curs_set(1);
    main();
}

