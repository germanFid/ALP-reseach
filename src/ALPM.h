#ifndef ALPM_H
#define ALPM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include "csv_table.h"

#define COMPRESS_EVERY_VALUE 100
#define BALANCED 101
#define BEST_ANALYZED 102
#define FIXED 103
#define CONTINUOUS 104
#define ONE_COLUMN 105
#define ALL_COLUMNS 106
#define MAX_ERR 0.00000000000001

typedef struct _decimals DECIMALS;

typedef struct _pow POW10;
struct _pow
{
    unsigned int neg;
    unsigned int pos;
};

POW10 ALPM_calculatepow(CsvTable* table, int option1, int option2, int column);

void ALPM_columntoi2(CsvTable* table, POW10* pow, void** out_array, int column);
void ALPM_tabletoi2(CsvTable* table, POW10* pow, void** out_array);
void ALPM_columntoi2_ce_se(CsvTable* table, POW10* pow, int** out_array, int** err_array, int column);
void ALPM_tabletoi2_ce_se(CsvTable* table, POW10* pow, int** out_array, int** err_array);

int delta_encode(CsvTable* table, int** input, int** signs, int options, int column, int of_r);
int delta_encode_ce(CsvTable* table, int** input, int** err, int** signs, int options, int column, int of_r);

#ifdef __cplusplus
};

#endif
#endif
