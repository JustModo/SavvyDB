#include "dbms.h"
#include <ncurses.h>

DatabaseNode *dbList = NULL;
DatabaseNode *dbNode = NULL;

// Create a new database
void create_database(DatabaseNode **head, const char *db_name)
{
    DatabaseNode *newDbNode = (DatabaseNode *)malloc(sizeof(DatabaseNode));
    strcpy(newDbNode->db.name, db_name);
    newDbNode->db.tables = NULL;
    newDbNode->next = *head;
    *head = newDbNode;
    printw("Database '%s' created.\n", db_name);
}

// Delete a database
void delete_database(DatabaseNode **head, const char *db_name)
{
    DatabaseNode *current = *head;
    DatabaseNode *prev = NULL;

    while (current != NULL && strcmp(current->db.name, db_name) != 0)
    {
        prev = current;
        current = current->next;
    }

    if (current == NULL)
    {
        printf("Database '%s' not found.\n", db_name);
        return;
    }

    if (prev == NULL)
    {
        *head = current->next;
    }
    else
    {
        prev->next = current->next;
    }

    free(current);
    printf("Database '%s' deleted.\n", db_name);
}

int list_databases(DatabaseNode *head, const char *choices[], int max_choices)
{
    int count = 0;
    DatabaseNode *temp = head;

    while (temp != NULL && count < max_choices - 1)
    {
        choices[count] = temp->db.name;
        count++;
        temp = temp->next;
    }

    choices[count] = "Go Back";
    count++;

    return count;
}

// Find a database by name
DatabaseNode *find_database(DatabaseNode *head, const char *db_name)
{
    while (head != NULL)
    {
        if (strcmp(head->db.name, db_name) == 0)
        {
            return head;
        }
        head = head->next;
    }
    printf("Database not found\n");
    return NULL;
}

void create_table(DatabaseNode *dbNode, const char *tableName)
{
    // Allocate memory for the new TableNode
    TableNode *newTableNode = malloc(sizeof(TableNode));
    if (!newTableNode)
    {
        perror("Failed to allocate memory for TableNode");
        return;
    }

    // Set table name and initialize with blank schema
    strcpy(newTableNode->table.name, tableName);
    newTableNode->table.numColumns = 0; // No columns yet
    newTableNode->table.columns = NULL; // No column definitions
    newTableNode->table.numRows = 0;    // No rows initially
    newTableNode->table.rows = NULL;    // No row data

    // Add the new table to the database's table list
    newTableNode->next = dbNode->db.tables;
    dbNode->db.tables = newTableNode;

    printw("Table '%s' created with a blank schema.\n", newTableNode->table.name);
}

// Validate input value by column type
int validate_value(const char *value, ColumnType type)
{
    char *endptr;
    switch (type)
    {
    case INTEGER:
        strtol(value, &endptr, 10);
        return (*endptr == '\0');
    case FLOAT:
        strtof(value, &endptr);
        return (*endptr == '\0');
    case BOOLEAN:
        return (strcmp(value, "true") == 0 || strcmp(value, "false") == 0);
    case STRING:
        return 1;
    default:
        return 0;
    }
}

// Check for unique values
int is_value_unique(Table *table, int colIndex, const char *value)
{
    for (int i = 0; i < table->numRows; i++)
    {
        if (strcmp(table->rows[i].values[colIndex], value) == 0)
        {
            return 0;
        }
    }
    return 1;
}

// Add a row to the table
void add_row_to_table(DatabaseNode *dbNode, const char *table_name)
{
    TableNode *tableNode = dbNode->db.tables;

    while (tableNode && strcmp(tableNode->table.name, table_name) != 0)
    {
        tableNode = tableNode->next;
    }

    if (!tableNode)
    {
        printf("Table '%s' not found in database '%s'.\n", table_name, dbNode->db.name);
        return;
    }

    Table *table = &tableNode->table;
    Row newRow;
    newRow.values = malloc(table->numColumns * sizeof(char *));
    if (table->numColumns == 0)
    {
        printf("Schema not defined\n");
        return;
    }
    for (int i = 0; i < table->numColumns; i++)
    {
        char input[MAX_INPUT];
        Column currentColumn = table->columns[i];

        while (1)
        {
            printf("Enter value for %s: ", currentColumn.name);
            scanf("%s", input);

            if (!validate_value(input, currentColumn.type))
            {
                printf("Invalid value for %s. Try again.\n", currentColumn.name);
                continue;
            }
            if (currentColumn.isUnique && !is_value_unique(table, i, input))
            {
                printf("Value for %s must be unique. Try again.\n", currentColumn.name);
                continue;
            }

            break;
        }

        newRow.values[i] = strdup(input);
    }

    table->rows = realloc(table->rows, (table->numRows + 1) * sizeof(Row));
    table->rows[table->numRows] = newRow;
    table->numRows++;
    printf("Row added to table '%s'.\n", table_name);
}

// List all rows in a table
void list_rows_in_table(DatabaseNode *dbNode, const char *table_name)
{
    TableNode *tableNode = dbNode->db.tables;

    while (tableNode && strcmp(tableNode->table.name, table_name) != 0)
    {
        tableNode = tableNode->next;
    }

    if (!tableNode)
    {
        printf("Table '%s' not found in database '%s'.\n", table_name, dbNode->db.name);
        return;
    }

    Table *table = &tableNode->table;
    if (table->numColumns == 0)
    {
        printf("Table empty\n");
        return;
    }

    for (int i = 0; i < table->numColumns; i++)
    {
        printf("%s\t", table->columns[i].name);
    }
    printf("\n");

    for (int i = 0; i < table->numRows; i++)
    {
        for (int j = 0; j < table->numColumns; j++)
        {
            printf("%s\t", table->rows[i].values[j]);
        }
        printf("\n");
    }
}

void delete_row_from_table(DatabaseNode *dbNode, const char *table_name, int rowIndex)
{

    TableNode *tableNode = dbNode->db.tables;
    while (tableNode != NULL && strcmp(tableNode->table.name, table_name) != 0)
    {
        tableNode = tableNode->next;
    }

    if (tableNode == NULL)
    {
        printf("Table '%s' not found.\n", table_name);
        return;
    }

    Table *table = &tableNode->table;

    if (rowIndex < 0 || rowIndex >= table->numRows)
    {
        printf("Invalid row index: %d\n", rowIndex);
        return;
    }

    for (int i = 0; i < table->numColumns; i++)
    {
        free(table->rows[rowIndex].values[i]);
    }
    free(table->rows[rowIndex].values);

    for (int i = rowIndex; i < table->numRows - 1; i++)
    {
        table->rows[i] = table->rows[i + 1];
    }

    table->numRows--;

    table->rows = realloc(table->rows, table->numRows * sizeof(Row));

    if (table->numRows > 0 && table->rows == NULL)
    {
        printf("Memory reallocation failed.\n");
        return;
    }

    printf("Row %d deleted successfully from table '%s'.\n", rowIndex, table_name);
}

void update_row(DatabaseNode *dbNode, const char *table_name, int rowIndex)
{
    TableNode *tableNode = dbNode->db.tables;

    while (tableNode && strcmp(tableNode->table.name, table_name) != 0)
    {
        tableNode = tableNode->next;
    }

    if (!tableNode)
    {
        printf("Table '%s' not found in database '%s'.\n", table_name, dbNode->db.name);
        return;
    }

    Table *table = &tableNode->table;

    if (rowIndex < 0 || rowIndex >= table->numRows)
    {
        printf("Row index %d is out of range for table '%s'.\n", rowIndex, table_name);
        return;
    }

    Row *row = &table->rows[rowIndex];
    char input[MAX_INPUT];
    for (int i = 0; i < table->numColumns; i++)
    {
        Column column = table->columns[i];
        printf("Current value for %s (index %d): %s\n", column.name, i, row->values[i]);
        while (1)
        {
            printf("Enter new value for %s: ", column.name);
            scanf("%s", input);

            if (!validate_value(input, column.type))
            {
                printf("Invalid value for %s. Try again.\n", column.name);
                continue;
            }

            if (column.isUnique && !is_value_unique(table, i, input) && strcmp(input, row->values[i]))
            {
                printf("Value for %s must be unique. Try again.\n", column.name);
                continue;
            }

            free(row->values[i]);
            row->values[i] = strdup(input);
            break;
        }
    }

    printf("Row %d updated in table '%s'.\n", rowIndex, table_name);
}

void free_table(Table *table)
{
    for (int i = 0; i < table->numRows; i++)
    {
        for (int j = 0; j < table->numColumns; j++)
        {
            free(table->rows[i].values[j]);
        }
        free(table->rows[i].values);
    }
    free(table->rows);

    free(table->columns);
}

int list_tables(DatabaseNode *dbNode, const char **choices)
{
    if (!dbNode)
    {
        printf("No database provided.\n");
        return -1;
    }

    TableNode *current = dbNode->db.tables;
    int count = 0;

    while (current)
    {
        if (count < 50)
        {
            choices[count] = current->table.name;
            count++;
        }
        current = current->next;
    }

    choices[count++] = "Go Back";
    return count;
}

// Function to delete a table from the database
void delete_table(DatabaseNode *dbNode, const char *table_name)
{
    TableNode *current = dbNode->db.tables;
    TableNode *previous = NULL;

    // Find the table to delete
    while (current && strcmp(current->table.name, table_name) != 0)
    {
        previous = current;
        current = current->next;
    }

    if (!current)
    {
        printf("Table '%s' not found in database '%s'.\n", table_name, dbNode->db.name);
        return;
    }

    // Remove the table node from the linked list
    if (previous)
    {
        previous->next = current->next;
    }
    else
    {
        dbNode->db.tables = current->next;
    }

    // Free memory of the table
    free_table(&current->table);
    free(current);

    printf("Table '%s' deleted from database '%s'.\n", table_name, dbNode->db.name);
}

// Function to write a table and its schema to a file
void write_table(FILE *file, Table *table)
{
    // Write table name, number of columns, and number of rows
    fprintf(file, "%s %d %d\n", table->name, table->numColumns, table->numRows);

    // Write column definitions (schema)
    for (int i = 0; i < table->numColumns; i++)
    {
        fprintf(file, "%s %d %d\n", table->columns[i].name, table->columns[i].type, table->columns[i].isUnique);
    }

    // Write rows of data
    for (int i = 0; i < table->numRows; i++)
    {
        for (int j = 0; j < table->numColumns; j++)
        {
            fprintf(file, "%s ", table->rows[i].values[j]);
        }
        fprintf(file, "\n"); // New line for each row
    }
}

// Function to write the entire database, including tables, to a file
void write_database_to_file(DatabaseNode *dbNode, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Failed to open file");
        return;
    }

    // Write the database name
    fprintf(file, "%s\n", dbNode->db.name);

    // Loop through each table in the database and write it
    TableNode *tableNode = dbNode->db.tables;
    while (tableNode)
    {
        write_table(file, &tableNode->table);
        tableNode = tableNode->next;
    }

    fclose(file);
    printf("Database '%s' written to file '%s'.\n", dbNode->db.name, filename);
}

void read_database_from_file(const char *filename, DatabaseNode **dbList)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        return;
    }

    // Read the database name
    char dbName[MAX_INPUT];
    if (fscanf(file, "%s", dbName) != 1)
    {
        perror("Failed to read database name");
        fclose(file);
        return;
    }

    // Create a new DatabaseNode
    DatabaseNode *dbNode = malloc(sizeof(DatabaseNode));
    if (!dbNode)
    {
        perror("Failed to allocate memory");
        fclose(file);
        return;
    }

    // Initialize the new DatabaseNode
    strcpy(dbNode->db.name, dbName);
    dbNode->db.tables = NULL;
    dbNode->next = NULL;

    // Add the new DatabaseNode to the list
    dbNode->next = *dbList;
    *dbList = dbNode;

    // Read tables
    TableNode **currentTableNode = &dbNode->db.tables;
    while (!feof(file))
    {
        char tableName[MAX_INPUT];
        int numColumns, numRows;

        // Read the table name, number of columns, and number of rows
        if (fscanf(file, "%s %d %d", tableName, &numColumns, &numRows) != 3)
        {
            if (feof(file))
                break;
            perror("Failed to read table metadata");
            fclose(file);
            return;
        }

        // Initialize the table
        Table table;
        strcpy(table.name, tableName);
        table.numColumns = numColumns;
        table.numRows = numRows;

        // Allocate memory for columns
        table.columns = malloc(numColumns * sizeof(Column));
        if (!table.columns)
        {
            perror("Failed to allocate memory for columns");
            fclose(file);
            return;
        }

        // Read each column's details
        for (int i = 0; i < numColumns; i++)
        {
            int type, isUnique;
            if (fscanf(file, "%s %d %d", table.columns[i].name, &type, &isUnique) != 3)
            {
                perror("Failed to read column details");
                fclose(file);
                return;
            }
            table.columns[i].type = (ColumnType)type;
            table.columns[i].isUnique = isUnique;
        }

        // Allocate memory for rows
        table.rows = malloc(numRows * sizeof(Row));
        if (!table.rows)
        {
            perror("Failed to allocate memory for rows");
            fclose(file);
            return;
        }

        // Read row data
        for (int i = 0; i < numRows; i++)
        {
            table.rows[i].values = malloc(numColumns * sizeof(char *));
            if (!table.rows[i].values)
            {
                perror("Failed to allocate memory for row values");
                fclose(file);
                return;
            }

            for (int j = 0; j < numColumns; j++)
            {
                table.rows[i].values[j] = malloc(MAX_INPUT * sizeof(char));
                if (!table.rows[i].values[j])
                {
                    perror("Failed to allocate memory for row value");
                    fclose(file);
                    return;
                }
                if (fscanf(file, "%s", table.rows[i].values[j]) != 1)
                {
                    perror("Failed to read row value");
                    fclose(file);
                    return;
                }
            }
        }

        // Add the new TableNode to the DatabaseNode
        TableNode *newTableNode = malloc(sizeof(TableNode));
        if (!newTableNode)
        {
            perror("Failed to allocate memory for TableNode");
            fclose(file);
            return;
        }

        newTableNode->table = table;
        newTableNode->next = NULL;
        *currentTableNode = newTableNode;
        currentTableNode = &newTableNode->next;
    }

    fclose(file);
    printf("Database '%s' read from file '%s'.\n", dbNode->db.name, filename);
}

Table *find_table(DatabaseNode *dbNode, const char *tableName)
{
    TableNode *current = dbNode->db.tables;
    while (current)
    {
        if (strcmp(current->table.name, tableName) == 0)
        {
            return &current->table;
        }
        current = current->next;
    }
    return NULL; // Table not found
}

int parse_column_type(const char *typeStr)
{
    if (strcmp(typeStr, "STRING") == 0)
    {
        return STRING;
    }
    else if (strcmp(typeStr, "INTEGER") == 0)
    {
        return INTEGER;
    }
    else if (strcmp(typeStr, "BOOLEAN") == 0)
    {
        return BOOLEAN;
    }
    else if (strcmp(typeStr, "FLOAT") == 0)
    {
        return FLOAT;
    }
    return -1; // Invalid type
}

void update_table_schema(DatabaseNode *dbNode, const char *tableName, const char *schemaInput)
{
    // Find the table by name
    Table *table = find_table(dbNode, tableName);
    if (!table)
    {
        fprintf(stderr, "Table '%s' not found.\n", tableName);
        return;
    }

    // Break the schemaInput into lines
    char *inputCopy = strdup(schemaInput);
    char *line = strtok(inputCopy, "\n");

    // Count lines to know how many columns we will need
    int columnCount = 0;
    while (line)
    {
        columnCount++;
        line = strtok(NULL, "\n");
    }

    free(inputCopy);
    inputCopy = strdup(schemaInput); // Reset input

    // Allocate space for the columns
    table->columns = realloc(table->columns, columnCount * sizeof(Column));
    if (!table->columns)
    {
        perror("Failed to allocate memory for columns");
        free(inputCopy);
        return;
    }

    // Parse each line
    int index = 0;
    line = strtok(inputCopy, "\n");
    while (line)
    {
        char columnName[MAX_INPUT];
        char columnTypeStr[MAX_INPUT];
        char uniqueFlag[MAX_INPUT] = "";

        // Extract column name, type, and optional unique flag
        int parts = sscanf(line, "%s %s %s", columnName, columnTypeStr, uniqueFlag);
        if (parts < 2)
        {
            fprintf(stderr, "Invalid column definition: %s\n", line);
            free(inputCopy);
            return;
        }

        // Set column name and type
        strcpy(table->columns[index].name, columnName);
        int columnType = parse_column_type(columnTypeStr);
        if (columnType == -1)
        {
            fprintf(stderr, "Invalid column type: %s\n", columnTypeStr);
            free(inputCopy);
            return;
        }
        table->columns[index].type = columnType;

        // Check if the column is unique
        table->columns[index].isUnique = (parts == 3 && strcmp(uniqueFlag, "unique") == 0) ? 1 : 0;

        index++;
        line = strtok(NULL, "\n");
    }

    table->numColumns = columnCount;
    free(inputCopy);
    printf("Table '%s' schema updated with %d columns.\n", table->name, columnCount);
}