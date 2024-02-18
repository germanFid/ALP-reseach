#include <stdio.h>
#include <stdlib.h>
#include <math.h>           
#include "csv.h"
#include "ALPM.h"

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
    /* Open .csv file and create handle */
    CsvHandle handle = csv_fopend("../src/test.csv");
    if (!handle)
    {
        printf("CsvHandle failed to create\n");
        exit(EXIT_FAILURE);
    }

    /* Get first row and number of fields (columns) in it */
    char* row = csv_readrow(handle);
    int num_fields = csv_getnumfields(row, handle);

    /* Allocate memory for arrays to store input data, compressed integers and decompressed data */
    double* in_array = (double*)malloc(sizeof(double) * num_fields);
    double* out_array = (double*)malloc(sizeof(double) * num_fields);
    long long* compressed_int = (long long*)malloc(sizeof(long long) * num_fields);
    if (!in_array || !out_array || !compressed_int)
    {
        free(in_array);
        free(out_array);
        free(compressed_int);
        exit(EXIT_FAILURE);
    }

    /* Calculate powers of 10 which will be used in ALP algorithm */
    POW10 POW10 = ALPM_calculatepow(row, num_fields, COMPRESS_EVERY_VALUE, NULL);
    printf("calculated powers of 10: %d, %d\n\n", POW10.neg, POW10.pos);

    /*  C = INPUT * 10^p * 10^-n
        DEC = C * 10^n * 10^-p

        (p = _pow->pos, n = _pow-neg) */

    for (int i = 0; i < num_fields; i++)
    {
        in_array[i] = atof(csv_readfield(row, handle));

        compressed_int[i] = llrint(in_array[i] * F10[POW10.pos] * i_F10[POW10.neg]);

        out_array[i] = compressed_int[i] * F10[POW10.neg] * i_F10[POW10.pos];

        printf("input =              %.20f \n", in_array[i]);
        printf("compressed integer = %Ii \n", compressed_int[i]);
        printf("decompressed value = %.20f \n", out_array[i]);
        printf("=====================================\n");
    }

    /* Close handle, free memory */
    csv_close(handle);
    free(in_array);
    free(out_array);
    free(compressed_int);
    return EXIT_SUCCESS;
}




