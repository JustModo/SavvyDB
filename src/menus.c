#include <string.h>
#include "menus.h"
#include "dbms.h"

#define MAX_INPUT 50

void display_menu(const char *title, const char *choices[], int num_choices, int *highlight)
{
    clear();
    mvprintw(0, 0, "%s", title);
    mvprintw(1, 0, "Use arrow keys to move, press Enter to select.");

    for (int i = 0; i < num_choices; i++)
    {
        if (i == *highlight)
        {
            attron(A_REVERSE);
        }
        mvprintw(i + 3, 0, choices[i]);
        attroff(A_REVERSE);
    }

    refresh();
}

void handle_main_menu()
{
    int highlight = 0;
    int choice = 0;
    const char *choices[] = {
        "Select Database",
        "Create New Database",
        "Exit"};
    int num_choices = sizeof(choices) / sizeof(choices[0]);

    while (1)
    {
        display_menu("Welcome to SavvyDB", choices, num_choices, &highlight);
        int ch = getch();

        switch (ch)
        {
        case KEY_UP:
            if (highlight > 0)
            {
                highlight--;
            }
            break;
        case KEY_DOWN:
            if (highlight < num_choices - 1)
            {
                highlight++;
            }
            break;
        case 10: // Enter key
            switch (highlight)
            {
            case 0:
                handle_database_menu();
                break;
            case 1:
                clear();
                mvprintw(0, 0, "Enter Database Name: ");
                char db_name[MAX_INPUT];
                echo();
                getstr(db_name);
                noecho();

                mvprintw(1, 0, "Enter Password: ");
                char password[MAX_INPUT];
                echo();
                getstr(password);
                noecho();
                printf("\n");

                create_database(&dbList, db_name);
                printw("Press any key to go back to the menu...\n");
                refresh();
                getch();
                break;
            case 2:
                return;
            }
            break;
        }
    }
}

void handle_database_menu()
{
    int highlight = 0;
    int choice = 0;
    const char *choices[50 + 1];

    int num_choices = list_databases(dbList, choices, sizeof(choices) / sizeof(choices[0]));

    while (1)
    {
        display_menu("Select a Database", choices, num_choices, &highlight);
        int ch = getch();

        switch (ch)
        {
        case KEY_UP:
            if (highlight > 0)
            {
                highlight--;
            }
            break;
        case KEY_DOWN:
            if (highlight < num_choices - 1)
            {
                highlight++;
            }
            break;
        case 10:
            if (highlight == num_choices - 1)
                return;
            else
            {
                dbNode = find_database(dbList, choices[highlight]);
                handle_table_menu();
            }
            break;
        }
    }
}

void handle_table_menu()
{
    int highlight = 0;
    int num_choices = 4;
    const char *choices[] = {
        "View Tables",
        "Create Table",
        "Open Playground",
        "Go Back"};

    char dispText[100];

    snprintf(dispText, sizeof(dispText), "Select Option in %s", dbNode->db.name);

    while (1)
    {
        display_menu(dispText, choices, num_choices, &highlight);
        int ch = getch();

        switch (ch)
        {
        case KEY_UP:
            if (highlight > 0)
            {
                highlight--;
            }
            break;
        case KEY_DOWN:
            if (highlight < num_choices - 1)
            {
                highlight++;
            }
            break;
        case 10: // Enter key
            if (highlight == num_choices - 1)
            {
                return;
            }
            else if (highlight == 0)
            {
                view_tables();
            }
            else if (highlight == 1)
            {
                clear();
                mvprintw(0, 0, "Enter Table Name: ");
                char table_name[MAX_INPUT];
                echo();
                getstr(table_name);
                noecho();
                refresh();
                clear();
                create_table(dbNode, table_name);
                printw("Press any key to go back to the menu...\n");
                refresh();
                getch();
            }
            else if (highlight == 2)
            {
                open_playground();
            }
            break;
        }
    }
}

void view_tables()
{
    int highlight = 0;
    const char *choices[50 + 1];
    int num_choices = list_tables(dbNode, choices);

    while (1)
    {
        refresh();
        display_menu("Select a Table", choices, num_choices, &highlight);
        int ch = getch();

        switch (ch)
        {
        case KEY_UP:
            if (highlight > 0)
            {
                highlight--;
            }
            break;
        case KEY_DOWN:
            if (highlight < num_choices - 1)
            {
                highlight++;
            }
            break;
        case 10: // Enter key
            if (highlight == num_choices - 1)
                return; // Go Back
            else
            {
                char *selected_table = (char *)choices[highlight];
                handle_record_menu(selected_table);
            }
            break;
        }
    }
}

void open_playground()
{
    clear();
    mvprintw(0, 0, "Welcome to the Playground!");
    char user_input[MAX_INPUT];
    mvprintw(2, 0, "ID\tName\tAddress");
    mvprintw(3, 0, "1\tTom\t123");
    mvprintw(4, 0, "2\tBob\t456");
    mvprintw(5, 0, "3\tRam\t789\n");
    printw("Enter your queries below:\n");
    echo();
    getstr(user_input);
    noecho();
    printw("\nExplanation of query if valid\n");
    printw("Press any key to go back to the menu...");
    refresh();
    getch();
}

void handle_record_menu(const char *table_name)
{
    int highlight = 0;
    int choice = 0;
    int num_choices = 6;
    const char *choices[] = {
        "Create Record",
        "Read Records",
        "Update Record",
        "Delete Record",
        "Edit Schema",
        "Go Back"};

    while (1)
    {
        display_menu(table_name, choices, num_choices, &highlight);
        int ch = getch();

        switch (ch)
        {
        case KEY_UP:
            if (highlight > 0)
            {
                highlight--;
            }
            break;
        case KEY_DOWN:
            if (highlight < num_choices - 1)
            {
                highlight++;
            }
            break;
        case 10:
            if (highlight == 5)
                return;
            else if (highlight == 4)
            {
                clear();
                mvprintw(0, 0, "Enter Schema:\n");
                char schema[200];
                echo();
                getstr(schema);
                noecho();
                refresh();
                update_table_schema(dbNode, table_name, schema);
                printw("Press any key to go back to the menu...");
                getch();
            }
            else if (highlight == 0)
            {
                clear();
                echo();
                add_row_to_table(dbNode, table_name);
                noecho();
                write_all_databases_to_file(dbList, "db.txt");
                getch();
            }
            else if (highlight == 1)
            {
                clear();
                echo();
                list_rows_in_table(dbNode, table_name);
                noecho();
                getch();
            }
            else if (highlight == 2)
            {
                char input[MAX_INPUT];
                int rowIndex;
                clear();
                echo();
                printw("Enter index of value to UPDATE:\n");
                getstr(input);

                if (sscanf(input, "%d", &rowIndex) != 1)
                {
                    printw("Invalid input. Please enter a valid number.\n");
                    getch();
                }
                else
                {
                    update_row(dbNode, table_name, rowIndex);
                    getch();
                }
                noecho();
            }
            else if (highlight == 3)
            {
                char input[MAX_INPUT];
                int rowIndex;
                clear();
                echo();
                printw("Enter index of value to DELETE:\n");
                getstr(input);

                if (sscanf(input, "%d", &rowIndex) != 1)
                {
                    printw("Invalid input. Please enter a valid number.\n");
                    getch();
                }
                else
                {
                    delete_row_from_table(dbNode, table_name, rowIndex);
                    getch();
                }

                noecho();
            }
            else
            {
                clear();
                printw("Performing action: %s on table '%s'\n", choices[highlight], table_name);
                printw("Press any key to go back to the menu...");
                refresh();
                getch();
            }
            break;
        }
    }
}
