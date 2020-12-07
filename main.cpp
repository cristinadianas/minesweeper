#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ncursesw/ncurses.h>
using namespace std;

#define Y_MAX 16
#define X_MAX 30

unsigned int board_rows, board_cols, total_mines;
WINDOW *winindex[Y_MAX][X_MAX];

#define CELL_WIDTH 5
#define CELL_HEIGHT 3
#define COUNTER_WIDTH 15
#define COUNTER_HEIGHT 3
#define TIMER_WIDTH 15
#define TIMER_HEIGHT 3
#define STATUS_WIDTH 30
#define STATUS_HEIGHT 9
#define DISTANCE_FROM_BOARD 3

#define COLOR_DEFAULT 0
#define COLOR_MARKEDCELL 1
#define GREEN_ON_BLACK 2
#define BLACK_ON_BLACK 3
#define RED_ON_BLACK 4
#define MAGENTA_ON_BLACK 5

void board_setup()
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
            } while (board_cols < 1 || board_cols > X_MAX);

            do {
                cout<<"HEIGHT = ";
                cin >> board_rows;
            } while (board_rows < 1 || board_rows > Y_MAX);

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

WINDOW *create_new_window(int height, int width, int starty, int startx)
{
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);		// 0, 0 gives default characters for the vertical and horizontal lines
    return local_win;
}
void show_in_window(int row, int col, char ch, int color_pair)
{
    wbkgd(winindex[row][col], COLOR_PAIR(color_pair));
    mvwaddch(winindex[row][col], CELL_HEIGHT / 2, CELL_WIDTH / 2, ch);
    wrefresh(winindex[row][col]);
}
void move_cursor(int row, int col)
{
    mvwaddch(winindex[row][col], CELL_HEIGHT / 2, CELL_WIDTH / 2 - 1, '[');
    mvwaddch(winindex[row][col], CELL_HEIGHT / 2, CELL_WIDTH / 2 + 1, ']');
    wrefresh(winindex[row][col]);
}
void take_a_step(int &initial_y, int &initial_x, int direction)
{
    switch (direction) {
        case KEY_LEFT:
            if (initial_x > 0)
            {
                mvwaddch(winindex[initial_y][initial_x], CELL_HEIGHT / 2, CELL_WIDTH / 2 - 1, ' ');
                mvwaddch(winindex[initial_y][initial_x], CELL_HEIGHT / 2, CELL_WIDTH / 2 + 1, ' ');
                wrefresh(winindex[initial_y][initial_x]);
                move_cursor(initial_y, --initial_x);
            }
            break;
        case KEY_RIGHT:
            if (initial_x < board_cols - 1)
            {
                mvwaddch(winindex[initial_y][initial_x], CELL_HEIGHT / 2, CELL_WIDTH / 2 - 1, ' ');
                mvwaddch(winindex[initial_y][initial_x], CELL_HEIGHT / 2, CELL_WIDTH / 2 + 1, ' ');
                wrefresh(winindex[initial_y][initial_x]);
                move_cursor(initial_y, ++initial_x);
            }
            break;
        case KEY_UP:
            if (initial_y > 0)
            {
                mvwaddch(winindex[initial_y][initial_x], CELL_HEIGHT / 2, CELL_WIDTH / 2 - 1, ' ');
                mvwaddch(winindex[initial_y][initial_x], CELL_HEIGHT / 2, CELL_WIDTH / 2 + 1, ' ');
                wrefresh(winindex[initial_y][initial_x]);
                move_cursor(--initial_y, initial_x);
            }
            break;
        case KEY_DOWN:
            if (initial_y < board_rows - 1)
            {
                mvwaddch(winindex[initial_y][initial_x], CELL_HEIGHT / 2, CELL_WIDTH / 2 - 1, ' ');
                mvwaddch(winindex[initial_y][initial_x], CELL_HEIGHT / 2, CELL_WIDTH / 2 + 1, ' ');
                wrefresh(winindex[initial_y][initial_x]);
                move_cursor(++initial_y, initial_x);
            }
            break;
        }
}
void sleep(float seconds)
{
    clock_t start_clock = clock();
    float seconds_ahead = seconds * CLOCKS_PER_SEC;
    // do nothing until the elapsed time has passed
    while (clock() < start_clock + seconds_ahead);
    return;
}

int num_marked_neighb(int row, int col, bool marked[][X_MAX])
{
    int neighbours = 0;
    for (int y = row - 1; y <= row + 1; y++)
        for (int x = col - 1; x <= col + 1; x++)
            if (marked[y][x] == TRUE && valid_cell(y, x) == TRUE)
                neighbours++;
    return neighbours;
}
void zero_pressed(int row, int col, int board_mines[][X_MAX], bool stepped[][X_MAX], bool marked[][X_MAX], int &steps)
{
    steps++;
    stepped[row][col] = TRUE;
    werase(winindex[row][col]);
    wrefresh(winindex[row][col]);
    for (int y = row - 1; y <= row + 1; y++)
        for (int x = col - 1; x <= col + 1; x++)
            if (valid_cell(y, x) == TRUE && stepped[y][x] == FALSE && marked[y][x] == FALSE)
                if (board_mines[y][x] == 0)
                    zero_pressed(y, x, board_mines, stepped, marked, steps);
                else
                {
                    steps++;
                    stepped[y][x] = TRUE;
                    show_in_window(y, x, '0' + board_mines[y][x], GREEN_ON_BLACK);
                }
}
void new_cell_pressed(int row, int col, int board_mines[][X_MAX], bool stepped[][X_MAX], bool marked[][X_MAX], bool &mine_pressed, int &steps)
{
    switch (board_mines[row][col])
    {
        case 0:
            zero_pressed(row, col, board_mines, stepped, marked, steps);
            break;
        case -1:
            stepped[row][col] = TRUE;
            mine_pressed = TRUE;
            break;
        default:
            steps++;
            stepped[row][col] = TRUE;
            show_in_window(row, col, '0' + board_mines[row][col], GREEN_ON_BLACK);
            break;
    }
}

int main()
{
    board_setup();
    bool exit_requested = false;
    do {
        int board_mines[Y_MAX][X_MAX] = {0}, y, x, laid_mines = 0;
        srand(time(NULL));

        for (y = 0; y < board_rows; y++)
            for (x = 0; x < board_cols; x++)
                if (total_mines - laid_mines != 0 && rand() < (RAND_MAX * (total_mines - laid_mines) / (board_rows * board_cols - (y * board_cols + x)))) {
                    board_mines[y][x] = -1;
                    laid_mines++;
                    for (int i = y - 1; i <= y + 1; i++)
                        for (int j = x - 1; j <= x + 1; j++)
                            if ((i != y || j != x) && board_mines[i][j] != -1 && valid_cell(i, j) == TRUE)
                                board_mines[i][j]++;
                }
// Enter NCURSES mode
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        curs_set(0);
        refresh();
//Initialize colors
        start_color();
        short color_markedcell = COLOR_YELLOW;
        if (can_change_color() == TRUE) {
            color_markedcell = 8;
            init_color(COLOR_MARKEDCELL, 1000, 369, 55); //Orange RGB: 255, 94, 14
        }
        init_pair(COLOR_MARKEDCELL, color_markedcell, COLOR_BLACK);
        init_pair(GREEN_ON_BLACK, COLOR_GREEN, COLOR_BLACK);
        init_pair(BLACK_ON_BLACK, COLOR_BLACK, COLOR_BLACK);
        init_pair(RED_ON_BLACK, COLOR_RED, COLOR_BLACK);
        init_pair(MAGENTA_ON_BLACK, COLOR_MAGENTA, COLOR_BLACK);

        for (y = 0; y < board_rows; y++)
            for (x = 0; x < board_cols; x++) {
                winindex[y][x] = create_new_window(CELL_HEIGHT, CELL_WIDTH, y * CELL_HEIGHT, x * CELL_WIDTH);
                wrefresh(winindex[y][x]);
            }
        int marked_cells = 0, steps = 0;
        bool stepped[Y_MAX][X_MAX] = {0}, marked[Y_MAX][X_MAX] = {0}, mine_pressed = FALSE;

        WINDOW *counter = create_new_window(COUNTER_HEIGHT, COUNTER_WIDTH, 0,board_cols * CELL_WIDTH + DISTANCE_FROM_BOARD);
        mvwprintw(counter, 1, 2, "%d/%d Mines ", marked_cells, total_mines);
        wrefresh(counter);

        WINDOW *status = create_new_window(STATUS_HEIGHT, STATUS_WIDTH, COUNTER_HEIGHT,board_cols * CELL_WIDTH + DISTANCE_FROM_BOARD);
        mvwaddstr(status, 4, 7, "You've got this!");
        wrefresh(status);

        clock_t startClock = clock();
        WINDOW *timer = create_new_window(TIMER_HEIGHT, TIMER_WIDTH, 0,board_cols * CELL_WIDTH + COUNTER_WIDTH + DISTANCE_FROM_BOARD);
        mvwprintw(timer, 1, 2, "%d seconds", (clock() - startClock) / CLOCKS_PER_SEC);
        wrefresh(timer);

        move_cursor(0, 0);
        x = 0;
        y = 0;

        int ch;
        while (!exit_requested && mine_pressed == FALSE && steps != (board_rows * board_cols) - total_mines) {
            while ((ch = getch()) != 'q' && ch != ' ' && ch != 'x') {
                if (ch != ERR)
                    take_a_step(y, x, ch);
                mvwprintw(timer, 1, 2, "%d seconds", (clock() - startClock) / CLOCKS_PER_SEC);
                wrefresh(timer);
            }
            switch (ch) {
                case ' ':
                    if (marked[y][x] == FALSE && stepped[y][x] == FALSE)
                        new_cell_pressed(y, x, board_mines, stepped, marked, mine_pressed, steps);
                    else if (stepped[y][x] == TRUE && num_marked_neighb(y, x, marked) == board_mines[y][x])
                        for (int i = y - 1; i <= y + 1; i++)
                            for (int j = x - 1; j <= x + 1; j++)
                                if (marked[i][j] == FALSE && stepped[i][j] == FALSE && valid_cell(i, j) == TRUE)
                                    new_cell_pressed(i, j, board_mines, stepped, marked, mine_pressed, steps);
                    break;
                case 'x':
                    if (stepped[y][x] == FALSE) {
                        if (marked[y][x] == FALSE) {
                            mvwprintw(counter, 1, 2, "%d/%d Mines ", ++marked_cells, total_mines);
                            if (marked_cells - 1 == total_mines) {
                                wbkgd(counter, COLOR_PAIR(RED_ON_BLACK));
                                beep();
                            }
                            wrefresh(counter);
                            show_in_window(y, x, 'x', COLOR_MARKEDCELL);
                            marked[y][x] = TRUE;
                        } else {
                            mvwprintw(counter, 1, 2, "%d/%d Mines ", --marked_cells, total_mines);
                            if (marked_cells == total_mines)
                                wbkgd(counter, COLOR_PAIR(COLOR_DEFAULT));
                            wrefresh(counter);
                            show_in_window(y, x, ' ', COLOR_DEFAULT);
                            marked[y][x] = FALSE;
                        }
                    }
                    break;
                case 'q':
                    exit_requested = true;
                    break;
            }
            move_cursor(y, x);
        }

        mvwaddstr(status, 4, 1, "                            ");
        mvwaddstr(status, 5, 3, "Press any key besides 'q'");
        mvwaddstr(status, 6, 9, "to continue.");
        wrefresh(status);

        mvwaddch(winindex[y][x], CELL_HEIGHT / 2, CELL_WIDTH / 2 - 1, ' ');
        mvwaddch(winindex[y][x], CELL_HEIGHT / 2, CELL_WIDTH / 2 + 1, ' ');
        wrefresh(winindex[y][x]);

        /* WIN: */ if (steps == (board_rows * board_cols) - total_mines) {   //WIN
            mvwaddstr(status, 3, 11, "YOU WON");
            wbkgd(status, COLOR_PAIR(GREEN_ON_BLACK));
            wrefresh(status);
            wbkgd(counter, COLOR_PAIR(GREEN_ON_BLACK));
            wrefresh(counter);
            wbkgd(timer, COLOR_PAIR(GREEN_ON_BLACK));
            wrefresh(timer);

            for (int i = 0; i < board_rows; i++)
                for (int j = 0; j < board_cols; j++)
                    if (marked[i][j] == FALSE && board_mines[i][j] == -1)
                        show_in_window(i, j, 'x', COLOR_MARKEDCELL);
            while ((ch = getch()) == ERR);
        } /* LOSE: */ else if (!exit_requested) {
            flash();
            mvwaddstr(status, 3, 11, "YOU LOST");
            wbkgd(status, COLOR_PAIR(RED_ON_BLACK));
            wrefresh(status);
            wbkgd(counter, COLOR_PAIR(RED_ON_BLACK));
            wrefresh(counter);
            wbkgd(timer, COLOR_PAIR(RED_ON_BLACK));
            wrefresh(timer);

            for (int i = 0; i < board_rows; i++)
                for (int j = 0; j < board_cols; j++)
                    if (marked[i][j] == TRUE && board_mines[i][j] != -1)
                        show_in_window(i, j, 'x', MAGENTA_ON_BLACK);
                    else if (marked[i][j] == FALSE && board_mines[i][j] == -1)
                        show_in_window(i, j, '*', RED_ON_BLACK);

            nodelay(stdscr, TRUE);
            while ((ch = getch()) == ERR) {
                for (int i = y - 1; i <= y + 1; i++)
                    for (int j = x - 1; j <= x + 1; j++)
                        if (valid_cell(i, j) == TRUE && board_mines[i][j] == -1 && stepped[i][j] == TRUE) {
                            wbkgd(winindex[i][j], COLOR_PAIR(BLACK_ON_BLACK));
                            wrefresh(winindex[i][j]);
                        }
                sleep(0.1);
                for (int i = y - 1; i <= y + 1; i++)
                    for (int j = x - 1; j <= x + 1; j++)
                        if (valid_cell(i, j) == TRUE && board_mines[i][j] == -1 && stepped[i][j] == TRUE) {
                            wbkgd(winindex[i][j], COLOR_PAIR(RED_ON_BLACK));
                            wrefresh(winindex[i][j]);
                        }
                sleep(0.1);
            }
        }
        if (ch == 'q')
            exit_requested = true;
    } while (!exit_requested);

    endwin();
    return 0;
}
