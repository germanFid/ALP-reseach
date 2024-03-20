#include <stdio.h>
#include <stdlib.h>
#include "binrw_new.h"

int calculate_shift(unsigned int *data, int num_elements) 
{
    int maxShift = sizeof(unsigned int) * 8; 

    for (int i = 0; i < num_elements; i++) 
    {
        unsigned int value = data[i];
        int shift = 0;

        while ((value & 0x80000000) == 0 && shift < maxShift) 
        {
            value <<= 1;
            shift++;
        }

        if (shift < maxShift)
        {
            maxShift = shift;
        }
    }

    return maxShift;
}

void shift_and_write(unsigned int *data, int num_elements, int shift_amount, FILE *file) 
{
    unsigned char buffer = 0;
    int bitsWritten = 0;

    printf("shift = %d\n", shift_amount);

    for (int i = 0; i < num_elements; i++) 
    {
        printf("-> %d\n", data[i]);
        unsigned int shiftedValue = data[i] >> shift_amount;

        buffer |= (shiftedValue & 0xFF);
        bitsWritten += 8 - shift_amount;

        if (bitsWritten >= 8) 
        {
            fwrite(&buffer, 1, 1, file);
            buffer = 0;
            bitsWritten = 0;
        } 
        
        else 
        {
            buffer <<= shift_amount;
        }
    }

    /* if some bits left, write them */
    if (bitsWritten > 0) 
    {
        fwrite(&buffer, 1, 1, file);
    }
}


int write_bin(struct BinData* data, char* filename, int n_rows, int n_cols)
{
    FILE* file = fopen(filename, "wb");
    if (!file)
    {
        return -1; // TODO: use actual error code constant
    }

    fwrite(&data->prefix._val, 1, 1, file); // write prefix
                                           // (comp_type: 1, n_columns: 7)


    // write core (moved to conditions)
    // fwrite(data->core.n_rows_block, 4, 1, file);
    // fwrite(data->core.delta, 4, 1, file);
    // fwrite(data->core.shift, 1, 1, file);

    /*
        if seq compression 
        then write values (n = cols * rows)
    */
    if (data->prefix._prefix.compression_type)
    {
        int n_values = n_rows * n_cols;
        int shift = calculate_shift(data->values, n_values);

        fwrite(&data->core.n_rows_block, 4, 1, file);
        fwrite(&data->core.delta, 4, 1, file);
        fwrite(&shift, 1, 1, file);

        shift_and_write(data->values, n_values, shift, file);
    }

    /*
        if col compression
        then write {core, values (n = rows)} * cols
    */
    else
    {
        // fclose(file);
        // exit(0);
        int n_values = n_rows;

        for (int i = 0; i < n_cols; i++)
        {
            int shift = calculate_shift(data->col_values[i], n_values);

            fwrite(&data->core.n_rows_block, 4, 1, file);
            fwrite(&data->core.delta, 4, 1, file);
            fwrite(&shift, 1, 1, file);

            shift_and_write(data->col_values[i], n_values, shift, file);
        }
    }

    // write doubles

    fclose(file);
    return 0;
}

