#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"      
#include "csv.h"
#include "csv_table.h"
#include "ALPM.h"
#include "binrw.h"

int main()
{
    /* Open .csv file and create handle */
    CsvHandle handle = csv_fopend("../../../src/test.csv");
    if (handle == NULL)
    {
        printf("CsvHandle failed to create\n");
        exit(EXIT_FAILURE);
    }

    /* Read contents of .csv file into a table */
    CsvTable table = { 0, 0, NULL };
    csv_table_init(&table, handle, 3);
    csv_table_print(&table);
    
    /* Actual rows/fields read */
    int num_rows = table.num_rows;
    int num_fields = table.num_fields;

    /* Allocate memory for 2d arrays to store compressed integers and errors */
    int** compressed_int = (int**)malloc(num_rows * sizeof(int*));
    char** error = (char**)malloc(num_rows * sizeof(char*));
    char** signs = (char**)malloc(num_rows * sizeof(char*));
    for (int i = 0; i < num_rows; i++)
    {
        error[i] = (char*)malloc(num_fields * sizeof(char));
        compressed_int[i] = (char*)malloc(num_fields * sizeof(char));
        signs[i] = (char*)malloc(num_fields * sizeof(char));
    }

    /* Calculate powers of 10 which will be used in ALP algorithm */
    /* Then convert values in CsvTable to 2d arrays of integers (and errors) using ALP algorithm */

    //POW10 POW10 = ALPM_calculatepow(&table, BALANCED, CONTINUOUS, NULL);
    //ALPM_tabletoi2_ce_se(&table, &POW10, compressed_int, error);

    // /*
    POW10* POW10_col = (POW10*)malloc(sizeof(POW10) * num_fields);
    for (int i = 0; i < num_fields; i++)
    {
        POW10_col[i] = ALPM_calculatepow(&table, BALANCED, ONE_COLUMN, i + 1);
        ALPM_columntoi2_ce_se(&table, &POW10_col[i], compressed_int, error, signs, i + 1); 
    }
    // */

    printf("\nCOMPRESSED INTEGERS:\n");
    for (int i = 0; i < num_rows; i++)
    {
        for (int j = 0; j < num_fields; j++)
        {
            printf("%d ", compressed_int[i][j]);
        }
        printf("\n");
    }
    printf("\nERROR VALUES:\n");
    for (int i = 0; i < num_rows; i++)
    {
        for (int j = 0; j < num_fields; j++)
        {
            printf("%d ", error[i][j]);
        }
        printf("\n");
    }
    printf("\nSIGNS:\n");
    for (int i = 0; i < num_rows; i++)
    {
        for (int j = 0; j < num_fields; j++)
        {
            printf("%d ", signs[i][j]);
        }
        printf("\n");
    }
    printf("\nCALCULATED DELTAS:\n");

    //int delta = delta_encode_ce(&table, compressed_int, error, CONTINUOUS, NULL, 10);

    // /*
    int* deltas = (int*)malloc(sizeof(int) * num_fields);
    for (int i = 0; i < num_fields; i++)
    {
        deltas[i] = delta_encode_ce(&table, compressed_int, error, ONE_COLUMN, i + 1, 10);
        printf("%d ", deltas[i]);
    }
    // */
    printf("\n\nENCODED INTEGERS:\n");
    for (int i = 0; i < num_rows; i++)
    {
        for (int j = 0; j < num_fields; j++)
        {
            printf("%d ", compressed_int[i][j]);
        }
        printf("\n");
    }

    /* Close handles, free memory */
    csv_close(handle);
    csv_table_free(&table);
    for (int i = 0; i < num_rows; i++)
    {
        free(compressed_int[i]);
        free(error[i]);
    }
    free(compressed_int);
    free(error);

    // /*
    free(POW10_col);
    free(deltas);
    // */
    return EXIT_SUCCESS;
}




