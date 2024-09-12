#ifndef MENUS_H
#define MENUS_H

#include <ncurses.h>

void display_menu(const char *title, const char *choices[], int num_choices, int *highlight);
void handle_main_menu();
void handle_database_menu();
void handle_table_menu();
void view_tables();
void open_playground();
void handle_record_menu(const char *table_name);

#endif