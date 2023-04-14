#include <ncurses.h>
#include <string>
#include <iostream>

using namespace std;

string getscr() {
    char a[1024];
    scanw("%[^\n]", a);
    return string(a);
}

void printscr(string s) {
    printw("%s", s.c_str());
}

int main() {

    initscr();

    int scrLine, scrCol;
    getmaxyx(stdscr, scrLine, scrCol);
/*
    //move(scrLine / 2, scrCol / 2);
    mvprintw(scrLine / 2, scrCol / 2, "%d %d\n", scrLine, scrCol);
    
    getch();
*/

/*
    scrollok(stdscr, true);
    setscrreg(0, scrLine / 2);

    for (int i = 1; i <= scrLine; ++i)  printscr("Test!!!!!!!\n");

    printscr("exceed!!!\n");
    getch();
*/

/*
    printw("YES\n");
    refresh();
    scr_dump("a");

    getch();
    WINDOW* win = newwin(scrLine, scrCol, 0, 0);
    wprintw(win, "Hello!\n");
    wrefresh(win);
    
    getch();
    wclear(win);
    wrefresh(win);
    delwin(win);

 //   getch();

    scr_restore("a");
    refresh();
    getch();
*/


    endwin();

    return 0;
}