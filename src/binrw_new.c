#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "binrw_new.h"

static int calculate_shift(unsigned int* data, int num_elements) 
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

static int shift_and_compress(unsigned int* buf, int buf_size, int* data, int data_size, int shift)
{
    int shifted_size = BUF_ELEM_SZ - shift;
    printf("shifted_size: %d\n", shifted_size);

    if(data_size * shifted_size > buf_size * BUF_ELEM_SZ)
    {
        return -1;
    }

    int buf_left = BUF_ELEM_SZ;
    int bit_remains = 0; // number of remaining bits to write before blocks

    int data_i = 0; // current index of data array
    for (int i = 0; i < buf_size; i++) // buf_size; i++)
    {
        printf("\n\n\t=== ITERATION %d ===\n", i);
        if (bit_remains)
        {
            buf[i] |= ((0xFFFFFFFF << (shifted_size-bit_remains)) & data[data_i]) >> (shifted_size-bit_remains);
            printf("written remains: %u\n", buf[i]);

            buf_left -= bit_remains;
            bit_remains = 0;
            data_i++;
        }

        if (data_i >= data_size)
        {
            break;
        }

        // write full blocks if available
        while (buf_left - shifted_size >= shifted_size)
        {
            printf("buf_left: %d\n", buf_left);
            buf[i] |= data[data_i];
            printf("%d -> %u\n", data_i, buf[i]);
            buf[i] = buf[i] << shifted_size;
            data_i++;

            buf_left -= shifted_size;
        }

        // write last block
        if (buf_left >= shifted_size)
        {
            buf[i] |= data[data_i];
            printf("last block: %d -> %u\n", data_i, buf[i]);
            buf_left -= shifted_size;
            data_i++;
        }

        // write partial data block
        if (buf_left > 0)
        {
            buf[i] = buf[i] << buf_left;
            unsigned int new_val = data[data_i] & (0xFFFFFFFF >> (DATA_ELEM_SZ - buf_left));
            // printf("new: %u\n", new_val);
            buf[i] |= new_val;

            bit_remains = shifted_size - buf_left;
            printf("written partial: %d\n", buf_left);
            printf("remains: %d\n", bit_remains);
        }

        buf_left = BUF_ELEM_SZ;
    }

    printf("\t=== buf_left: %d\n", buf_left);

    return 0;
}

int write_bin(int** data, int data_cols, int data_rows, int compression_type, int* deltas, int** signs, int** errors, char* filename)
{
    FILE* file = fopen(filename, "wb");
    
    if (compression_type)
    {
        int* buf = malloc(sizeof(int) * data_rows);
        for (int i = 0; i < data_cols; i++)
        {
            int shift = calculate_shift(data[i], data_cols);
            int elem_bits_size = (sizeof(int) * 8) - shift;
            int n_elements = (elem_bits_size / 8 + (elem_bits_size % 8 ? 1 : 0)) * data_rows;
            
            shift_and_compress(buf, data_rows, data[i], data_rows, shift);

            // write elements
            for (int j = 0; j < n_elements; j++)
            {
                fwrite(&buf[j], sizeof(int), 1, file);
            }

            fwrite(&deltas[i], sizeof(int), 1, file); // write delta

            // write signs
            int buf_signs_sz = data_rows / 32 + (data_rows % 32 ? 1 : 0);
            int* buf_signs = malloc(sizeof(int) * buf_signs_sz);
            shift_and_compress(buf, buf_signs_sz, signs[i], data_rows, 31);
            
            shift_and_compress(buf, buf_signs_sz, errors[i], data_rows, 31);
            free(buf_signs);
        }

        free(buf);
    }
}

