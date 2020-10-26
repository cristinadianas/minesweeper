#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ncursesw/ncurses.h>
using namespace std;

WINDOW *create_newwin(int height, int width, int starty, int startx);

unsigned int board_rows, board_cols, total_mines;

#define cell_width 5
#define cell_height 3
WINDOW *winindex[30][30];

bool valid_cell(int y, int x)
{
    if (y >= 0 && y < board_rows && x >= 0 && x < board_cols)
        return TRUE;
    return FALSE;
}
void num_neighbours (int v[][30], int y, int x)
{
    if (v[y-1][x-1]!=-1) v[y-1][x-1]++;
    if (v[y-1][x]!=-1) v[y-1][x]++;
    if (v[y-1][x+1]!=-1) v[y-1][x+1]++;
    if (v[y+1][x+1]!=-1) v[y+1][x+1]++;
    if (v[y+1][x]!=-1) v[y+1][x]++;
    if (v[y+1][x-1]!=-1) v[y+1][x-1]++;
    if (v[y][x-1]!=-1) v[y][x-1]++;
    if (v[y][x+1]!=-1) v[y][x+1]++;

//    unsigned int neighbours=0;
//    if (y!=0 && x!=0 && v[y-1][x-1]==-1) neighbours++;
//    if (y!=0 && v[y-1][x]==-1) neighbours++;
//    if (y!=0 && x!=board_cols-1 && v[y-1][x+1]==-1) neighbours++;
//    if (y!=board_rows-1 && x!=board_cols-1 && v[y+1][x+1]==-1) neighbours++;
//    if (y!=board_rows-1 && v[y+1][x]==-1) neighbours++;
//    if (y!=board_rows-1 && x!=0 && v[y+1][x-1]==-1) neighbours++;
//    if (x!=0 && v[y][x-1]==-1) neighbours++;
//    if (x!=board_cols-1 && v[y][x+1]==-1) neighbours++;
//    return neighbours;

//    for (short m=y-1; m<=y+1; m++)
//        for (short n=x-1; n<=x+1; n++)
//            if (v[m][n]==-1) neighbours++;
//    return neighbours;
}
void bsetup()
{
    int dif;
    cout<<"DIFFICULTY: \n 1 = Beginner (9x9 - 10 Mines)  \n 2 = Medium (16x16 - 40 Mines) \n 3 = Difficult (16x30 - 99 Mines) \n 4 = Custom (max: 30x30 - 900 Mines) \n \n";
    cin>>dif;
    cout<<endl;
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
            cout<<"WIDTH = ";
            cin >> board_cols;
            cout<<"HEIGHT = ";
            cin >> board_rows;
            cout<<"MINES = ";
            cin >> total_mines;
            cout<<endl;
            break;
    }

}
void move_cursor (int row, int column)
{
    wmove(winindex[row][column], cell_height/2, cell_width/2);
    wrefresh(winindex[row][column]);
}
void UDRL (int ch, int &starty, int &startx, int color)
{
    //wattroff(winindex[starty][startx], A_BOLD);
    switch (ch) {
        case KEY_LEFT:
            if (startx > 0)
            {
                wmove(winindex[starty][--startx], cell_height/2, cell_width/2);
                wattron(winindex[starty][startx], A_UNDERLINE);
                wrefresh(winindex[starty][startx]);
            }
            break;
        case KEY_RIGHT:
            if (startx < board_cols - 1)
            {
                wmove(winindex[starty][++startx], cell_height/2, cell_width/2);
                wattron(winindex[starty][startx], A_BOLD);
                wrefresh(winindex[starty][startx]);
            }
            break;
        case KEY_UP:
            if (starty > 0)
            {
                wmove(winindex[--starty][startx], cell_height/2, cell_width/2);
                wattron(winindex[starty][startx], A_BOLD);
                wrefresh(winindex[starty][startx]);
            }
            break;
        case KEY_DOWN:
            if (starty < board_rows - 1)
            {
                wmove(winindex[++starty][startx], cell_height/2, cell_width/2);
                wattron(winindex[starty][startx], A_UNDERLINE);
                wrefresh(winindex[starty][startx]);
            }
            break;
        }
}
void show_in_win(int y, int x, char ch)
{
    mvwaddch(winindex[y][x], cell_height/2, cell_width/2, ch);
    wmove(winindex[y][x], cell_height/2, cell_width/2);
    wrefresh(winindex[y][x]);
}
void ZERO_PRESSED (int row, int col, int board_mines[][30], int stepped[][30])
{
    stepped[row][col] = TRUE;
    wbkgd(winindex[row][col], COLOR_PAIR(3));
    wrefresh(winindex[row][col]);
    for (int y = row - 1; y <= row + 1; y++)
        for (int x = col - 1; x <= col + 1; x++)
            if (valid_cell(y, x) == TRUE && stepped[y][x] == FALSE)
                if (board_mines[y][x] == 0)
                    ZERO_PRESSED(y, x, board_mines, stepped);
                else
                {
                    stepped[y][x] = TRUE;
                    wbkgd(winindex[y][x], COLOR_PAIR(2));
                    wrefresh(winindex[y][x]);
                    show_in_win(y, x, '0' + board_mines[y][x]);
                }
    move_cursor(row, col);
}
void exZER0_PRESSED (int stepped[][30], int board_mines[][30], int y, int x)
{
//    stepped[y][x] = 1;
//    wbkgd(winindex[y][x], COLOR_PAIR(3));
//    wrefresh(winindex[y][x]);
//    int a=0, b=0;
//    bool repeat = 1;
//    while (repeat == 1)
//    {
//        repeat = 0;
//        for (a = 0; a < board_rows; a++)
//            for (b = 0; b < board_cols; b++)
//                if (stepped[a][b] == 0)
//                for (int i = a - 1; i <= a + 1; i++)
//                    for (int j = b - 1; j <= b + 1; j++)
//                    if (board_mines[i][j] == 0 && stepped[i][j] == 1 && i!=-1 && j!=-1 && i!=board_rows && j!= board_cols)
//                    {
//                        if (board_mines[a][b] == 0)
//                        {
//                            wbkgd(winindex[a][b], COLOR_PAIR(3));
//                            wrefresh(winindex[a][b]);
//                        }
//                        else
//                        {
//                            show_in_win(a, b, '0' + board_mines[a][b]);
//                            wbkgd(winindex[a][b], COLOR_PAIR(2));
//                            wrefresh(winindex[a][b]);
//                        }
//                        wrefresh(winindex[a][b]);
//                        stepped[a][b] = 1;
//                        repeat = 1;
//                    }
//    }
//    move_cursor(y, x);
}

int main()
{	WINDOW *my_win;
    bsetup();
    int board_mines[30][30] = {0}, stepped[30][30]={0};
    int y, x, laid_mines = 0;
    srand (time(NULL));

    for (y=0; y < board_rows; y++)
        for (x=0; x < board_cols; x++)
            if (total_mines - laid_mines != 0 && rand() < (RAND_MAX * (total_mines - laid_mines) / (board_rows * board_cols - (y * board_cols + x))))
            {
                board_mines[y][x]=-1;
                laid_mines++;
                num_neighbours(board_mines, y, x);
            }

//    for (y=0; y < board_rows; y++)
//        for (x=0; x < board_cols; x++)
//            if (board_mines[y][x] != -1)
//                board_mines[y][x] = num_neighbours(board_mines, y, x);

//    for (y=0; y < board_rows; y++)
//    {
//        for (x=0; x < board_cols; x++)
//            if (board_mines[y][x] == -1)  cout << "*  ";
//            else  cout << num_neighbours(board_mines, y, x) << "  ";
//        cout<<endl;
//    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    refresh();

    for (y = 0; y < board_rows ; y++)
        for (x = 0; x < board_cols; x++)
        {
            my_win = create_newwin(cell_height, cell_width, y*cell_height, x*cell_width);
            winindex[y][x]=my_win;
            //wattrset(my_win, A_UNDERLINE);
            wrefresh(my_win);
        }

    move_cursor(0, 0); x=0; y=0;
    start_color();
    short COLOR_MARKEDCELL = COLOR_YELLOW;
    if (can_change_color() == TRUE)
    {
        COLOR_MARKEDCELL = 8;
        init_color(COLOR_MARKEDCELL, 1000, 345, 0); //Orange
    }
    init_pair(1, COLOR_MARKEDCELL, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_BLACK);

    while (1)
    {
        int ch;
        while ((ch = getch()) != 'q' && ch != ' ' && ch != 'x')
            UDRL(ch, y, x, 2);
        if (ch == ' ' && board_mines[y][x] != -1 && board_mines[y][x] != 0)
            {
                wbkgd(winindex[y][x], COLOR_PAIR(2));
                wrefresh(winindex[y][x]);
                show_in_win(y, x, '0' + board_mines[y][x]);
            }
        else if (ch == ' ' && board_mines[y][x] == 0)
            ZERO_PRESSED(y, x, board_mines, stepped);
        else if (ch == 'x')
        {
            wbkgd(winindex[y][x], COLOR_PAIR(1));
            wrefresh(winindex[y][x]);
            show_in_win(y, x, 'x');
        }
        else break;
    }
    endwin();
    return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);		// 0, 0 gives default characters for the vertical and horizontal lines
    return local_win;
}


