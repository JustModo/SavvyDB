#include <ncurses.h>
#include "menus.h"

int main()
{
    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    handle_main_menu();

    endwin();
    return 0;
}
