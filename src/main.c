#include <ncurses.h>
#include "menus.h"
#include "dbms.h"

int main()
{
    read_database_from_file("db.txt", &dbList);
    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    handle_main_menu();

    endwin();
    return 0;
}
