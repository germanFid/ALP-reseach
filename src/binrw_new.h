#include "csv_table.h"

struct Prefix
{
    unsigned int compression_type: 1;
    unsigned int columns: 7;
};

union PrefixWrapper
{
    struct Prefix _prefix;
    unsigned char _val;
};


struct Core
{
    unsigned int n_rows_block;
    unsigned int delta;
    unsigned char shift;
};

struct BinData
{
    // unsigned char compression_type;
    union PrefixWrapper prefix;
    struct Core core;

    int* values;
    int** col_values;
    double* extras;
};

// int write_bin(struct BinData*, char*);
int write_bin(struct BinData* data, char* filename, int n_rows, int n_cols);
