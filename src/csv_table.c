#include <stdio.h>
#include <stdlib.h>
#include "csv_table.h"

void csv_table_init(CsvTable* table, CsvHandle handle, int num_rows)
{
    char* current_row = csv_readrow(handle);
    int num_fields = csv_getnumfields(current_row, handle);

    table->num_fields = num_fields;
    table->num_rows = num_rows;
    
    table->table = (char***) malloc(num_rows * num_fields * sizeof(char*));
    for (int i = 0; i < num_rows; i++)
    {
        table->table[i] = (char**) malloc(num_fields * sizeof(char*));
    }

    char* tmp;

    for(int i = 0; i < num_rows; i++)
    {
        for (int j = 0; j < num_fields; j++)
        {
            tmp = csv_readfield(current_row, handle);
            char* copy = malloc(strlen(tmp) + 1);
            strcpy(copy, tmp);
            table->table[i][j] = copy;
        }

        current_row = csv_readrow(handle);
        
        if (!current_row)
        {
            table->num_rows = i + 1;
        }
    }
}

void csv_table_free(CsvTable* table)
{
    for (int i = 0; i < table->num_rows; i++)
    {
        for (int j = 0; j < table->num_fields; j++)
        {
            free(table->table[i][j]);
        }
    }
    
    for (int i = 0; i < table->num_rows; i++)
    {
        free(table->table[i]);
    }
}

void csv_table_print(CsvTable* table)
{
    for (int i = 0; i < table->num_rows; i++)
    {
        for (int j = 0; j < table->num_fields; j++)
        {
            printf("%s ", table->table[i][j]);
        }
        printf("\n");
    }
}
