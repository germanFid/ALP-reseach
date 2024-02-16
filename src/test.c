#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "csv.h"

double i_F10[] = {  1.0, 
                    0.1, 
                    0.01, 
                    0.001, 
                    0.0001, 
                    0.00001, 
                    0.000001, 
                    0.0000001, 
                    0.00000001, 
                    0.000000001, 
                    0.0000000001, 
                    0.00000000001, 
                    0.000000000001, 
                    0.0000000000001, 
                    0.00000000000001, 
                    0.000000000000001, 
                    0.0000000000000001, 
                    0.00000000000000001, 
                    0.000000000000000001, 
                    0.0000000000000000001, 
                    0.00000000000000000001};

double F10[] = {    1.0, 
                    10.0, 
                    100.0, 
                    1000.0, 
                    10000.0, 
                    100000.0, 
                    1000000.0, 
                    10000000.0, 
                    100000000.0, 
                    1000000000.0, 
                    10000000000.0, 
                    100000000000.0, 
                    1000000000000.0, 
                    10000000000000.0,
                    100000000000000.0,
                    1000000000000000.0,
                    10000000000000000.0,
                    100000000000000000.0,
                    1000000000000000000.0,
                    10000000000000000000.0,
                    100000000000000000000.0};

int main()
{
    CsvHandle handle = csv_fopend("test.csv");
    if (!handle)
    {
        printf("CsvHandle failed to create\n");
        exit(EXIT_FAILURE);
    }
    char* row = csv_readrow(handle);
    int num_fields = csv_getnumfields(row, handle);

    double* in_array = (double*)malloc(sizeof(double) * num_fields);
    double* out_array = (double*)malloc(sizeof(double) * num_fields);
    int* compressed_int = (int*)malloc(sizeof(int) * num_fields);

    if (!in_array || !out_array || !compressed_int)
    {
        printf("Why...\n");
        free(in_array);
        free(out_array);
        free(compressed_int);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_fields; i++)
    {
        in_array[i] = atof(csv_readfield(row, handle));
        
        compressed_int[i] = round(in_array[i] * F10[14] * i_F10[10]);

        out_array[i] = compressed_int[i] * F10[10] * i_F10[14];

        printf("input = %.20f \n", in_array[i]);
        printf("compressed integer = %d \n", compressed_int[i]);
        printf("decompressed value = %.20f \n", out_array[i]);
        printf("=====================================\n");
    }

    csv_close(handle);
    free(in_array);
    free(out_array);
    free(compressed_int);

    return 0;
}






