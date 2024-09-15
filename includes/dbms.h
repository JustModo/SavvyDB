#ifndef DBMS_H
#define DBMS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 50

typedef enum
{
    INTEGER,
    STRING,
    BOOLEAN,
    FLOAT
} ColumnType;

typedef struct
{
    char name[MAX_INPUT];
    ColumnType type;
    int isUnique;
} Column;

typedef struct
{
    char **values;
} Row;

typedef struct
{
    char name[MAX_INPUT];
    Column *columns;
    Row *rows;
    int numColumns;
    int numRows;
} Table;

typedef struct TableNode
{
    Table table;
    struct TableNode *next;
} TableNode;

typedef struct
{
    char name[MAX_INPUT];
    TableNode *tables;
} Database;

typedef struct DatabaseNode
{
    Database db;
    struct DatabaseNode *next;
} DatabaseNode;

extern DatabaseNode *dbList;
extern DatabaseNode *dbNode;

void create_database(DatabaseNode **head, const char *db_name);
void delete_database(DatabaseNode **head, const char *db_name);
int list_databases(DatabaseNode *head, const char *choices[], int max_choices);
DatabaseNode *find_database(DatabaseNode *head, const char *db_name);

void create_table(DatabaseNode *dbNode, const char *tableName);
void list_rows_in_table(DatabaseNode *dbNode, const char *table_name);
void delete_table(DatabaseNode *dbNode, const char *table_name);
int list_tables(DatabaseNode *dbNode, const char **choices);

void add_row_to_table(DatabaseNode *dbNode, const char *table_name);
void delete_row_from_table(DatabaseNode *dbNode, const char *table_name, int rowIndex);
void update_row(DatabaseNode *dbNode, const char *table_name, int rowIndex);

void write_all_databases_to_file(DatabaseNode *dbList, const char *filename);
void read_database_from_file(const char *filename, DatabaseNode **dbList);
void update_table_schema(DatabaseNode *dbNode, const char *tableName, const char *schemaInput);

#endif
