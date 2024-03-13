struct prefix
{
    unsigned int compression_type: 1;
    unsigned int columns: 7;
};

struct core
{
    unsigned int n_rows_block: 4;
    unsigned int delta: 4;
    unsigned int shift: 1;
};

struct bin_data
{
    unsigned char compression_type;
    struct prefix prefix;
    struct core core;

    int* values;
    double* extras;
};