#include <stdio.h>
#include <stdlib.h>
#include <math.h>           
#include "csv.h"
#include "ALPM.h"
#include "binrw.h"

char*** csv_readtable(CsvHandle handle, int num_rows, int* read_rows, int* read_fields)
{
    char* current_row = csv_readrow(handle);
    int num_fields = csv_getnumfields(current_row, handle);

    *read_fields = num_fields;
    *read_rows = num_rows;
    
    char*** arr = (char***) malloc(num_rows * num_fields * sizeof(char*));
    for (int i = 0; i < num_rows; i++)
    {
        arr[i] = (char**) malloc(num_fields * sizeof(char*));
    }

    for(int i = 0; i < num_rows; i++)
    {
        for (int j = 0; j < num_fields; j++)
        {
            arr[i][j] = csv_readfield(current_row, handle);
        }
        
        current_row = csv_readrow(handle);
        
        /*
            - What's yo name?
            - Deez
            - Deez what?
            - Deez n**s
        */
        if (!current_row)
        {
            *read_rows = i + 1;
            return arr;
        }
    }

    return arr;
}

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
    CsvHandle handle = csv_fopend("test.csv");
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
    int* error = (int*)malloc(sizeof(int) * num_fields);
    if (!in_array || !out_array || !compressed_int || !error)
    {
        free(in_array);
        free(out_array);
        free(compressed_int);
        free(error);
        exit(EXIT_FAILURE);
    }

    /* Calculate powers of 10 which will be used in ALP algorithm */
    POW10 POW10 = ALPM_calculatepow(row, num_fields, BALANCED, NULL);
    printf("calculated powers of 10: %d, %d\n\n", POW10.neg, POW10.pos);

    /*  C = INPUT * 10^p * 10^-n
        DEC = C * 10^n * 10^-p

        (p = POW10->pos, n = POW10->neg) */

    for (int i = 0; i < num_fields; i++)
    {
        in_array[i] = atof(csv_readfield(row, handle));

        compressed_int[i] = llrint(in_array[i] * F10[POW10.pos] * i_F10[POW10.neg]);

        out_array[i] = compressed_int[i] * F10[POW10.neg] * i_F10[POW10.pos];

        printf("input =              %.20f \n", in_array[i]);
        printf("compressed integer = %Ii \n", compressed_int[i]);
        printf("decompressed value = %.20f \n", out_array[i]);

        if (fabs(out_array[i] - in_array[i]) > MAX_ERR)
            error[i] = 1;
        else
            error[i] = 0;
        printf("error? %d \n", error[i]);
        printf("=====================================\n");
    }

    /* Test binary writing
        1. Initialize block parameters 
            { bytes for element, BLOCK LENGTH, ERROR CODE }
        2. Create BinRWHandle
        3. Write block to binary file

        @ BLOCK:
        POW10->neg....POW10->pos....BLOCK LENGTH...DATA............................................REPEAT
        |- 1 byte -|..|- 1 byte -|..|- 4 bytes -|..|(sizeof(element in DATA) + 1) * BLOCK LENGTH|..REPEAT

        @ FINALLY:
        ERROR VALUES..........EOF
        |sizeof(double) * N|..EOF
    */

    BinBlocks blocks = init_blocks(sizeof(int), num_fields, 0b1);
    BinRWHandle bin_handle = bin_fopenf("out.bin", BINARY_WRITE, &blocks);
    bin_writeb(bin_handle, POW10.neg, POW10.pos, compressed_int, error, in_array);
    bin_close(bin_handle);

    /* Close handle, free memory */
    csv_close(handle);
    free(in_array);
    free(out_array);
    free(compressed_int);
    return EXIT_SUCCESS;
}




