#ifndef CSV_TABLE_H
#define CSV_TABLE_H

#include "csv.h"

typedef struct _CsvTable {
    int num_rows, num_fields;
    char*** table;
} CsvTable;

void csv_table_init(CsvTable* table, CsvHandle handle, int num_rows);
void csv_table_free(CsvTable* table);
void csv_table_print(CsvTable* table);

#endif /* CSV_TABLE_H */
