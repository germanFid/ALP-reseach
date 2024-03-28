#include "ALPM.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

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
                    0.00000000000000000001 };

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
                    100000000000000000000.0 };

struct _decimals
{
    unsigned int mean_p;
    unsigned int max_p;
    unsigned int min_p;
};

static DECIMALS get_decim(int options, int column, CsvTable* table)
{
    unsigned int current = 0;
    unsigned int max = 0;
    unsigned int min = 999;
    unsigned int mean = 0;
    size_t idx = 0;
    size_t count = 0;

    switch (options)
    {
    case CONTINUOUS:
    {
        for (size_t i = 0; i < table->num_rows; i++)
        {
            for (size_t j = 0; j < table->num_fields; j++)
            {
                current = strlen(strchr(table->table[i][j], '.')) - 1;
                max = current > max ? current : max;
                min = current > min ? min : current;
                count += current;
            }
        }
        mean = lrint((double)(count) / (table->num_rows * table->num_fields));
        DECIMALS decim = { mean, max, min };
        return decim;
    }
    case ONE_COLUMN:
    {
        for (size_t i = 0; i < table->num_rows; i++)
        {
            current = strlen(strchr(table->table[i][column-1], '.')) - 1;
            max = current > max ? current : max;
            min = current > min ? min : current;
            count += current;
        }
        mean = lrint((double)(count) / (table->num_rows));
        DECIMALS decim = { mean, max, min };
        return decim;
    }
    }
}

POW10 ALPM_calculatepow(CsvTable* table, int option1, int option2, int column)
{
    DECIMALS dec = get_decim(option2, column, table);

    switch (option1)
    {
    case COMPRESS_EVERY_VALUE:
        /*
        @ PROS:
            1.  It is guaranteed that every value will be compressed using ALP algorithm.
        @ CONS:
            1.  If the quantity of decimal places among values in some sampling differs significantly,
                we might end with many unnecessary big integers with "trailing zeros"
                which will lower the compression ratio.
        @ ALGORITHM:
            1.  If we want guaranteed compression of every value in some sampling,
                we first need to calculate negative power of 10 (_pow->neg) based on the biggest
                number of decimal places encountered in that sampling, then increase it by 6 (ONLY for 64-bit floats!).
            NOTE 1: absolute value of negative power should not exceed 10.
            NOTE 2: the difference between absolute values of positive and negative powers of 10 is required
                to be equal to the maximum number of decimal places in the sampling.
            2.  Calculate positive power of 10 (_pow->pos) based on the NOTE 2 above.
            3.  If positive power of 10 turns out to be bigger than 16, it should be set to 16 and the difference
                between calculated value and 16 should be subtracted from negative power.
        */
    {
        int pow_neg = (dec.max_p + 6) <= 10 ? (dec.max_p + 6) : 10;
        int pow_pos = pow_neg + dec.max_p;
        if (pow_pos > 16)
        {
            int diff = pow_pos - 16;
            pow_pos = 16;
            pow_neg -= diff;
        }
        POW10 pow = { pow_neg, pow_pos };
        return pow;
    }
    case BALANCED:
        /* 
        @ PROS:
            1.  This algorithm provides compression to smaller integers - w/o a lot of zeroes.
        @ CONS:
            1.  While this algorithm uses mean number of decimal places it is possible that 
                not all values in some input sampling will be compressed using ALP.
            2.  Mathematical properties of input samplings are not taken into account.
        @ ALGORITHM:
            1.  First calculate negative power of 10 (_pow->neg) based on the mean number
                of decimal places encountered in some sampling, then increase it by 6;
                (ONLY for 64-bit floats!).
            NOTE 1: absolute value of negative power should not exceed 10.
            NOTE 2: the difference between absolute values of positive and negative powers of 10 is required
                to be equal to the mean number of decimal places in the sampling.
            2.  Calculate positive power of 10 (_pow->pos) based on the NOTE 2 above.
            3.  If positive power of 10 turns out to be bigger than 16, it should be set to 16 and the difference
                between calculated value and 16 should be subtracted from negative power.
            4.  Compare max and mean number of found decimal places. If max is bigger
                than mean by 4 and more, subtract the difference from every power;  
         */
    {
        int pow_neg = (dec.mean_p + 6) <= 10 ? (dec.mean_p + 6) : 10;
        int pow_pos = pow_neg + dec.mean_p;
        if (pow_pos > 16)
        {
            int diff = pow_pos - 16;
            pow_pos = 16;
            pow_neg -= diff;
        }
        int balance_factor = dec.max_p - dec.mean_p;
        if (balance_factor > 4)
        {
            pow_pos -= balance_factor;
            pow_neg -= balance_factor;
        }
        POW10 pow = { pow_neg, pow_pos };
        return pow;
    }
    case BEST_ANALYZED:
        /* TODO: use mean, min and max precision + decide based on standard deviation of data */
    {
    }
    case FIXED:
        /* TODO: given args, not so useful ? */
    {
    }
    }
}

/*  Note that in the 2 functions below we don't need to check the 
    lrint() return value that might be 0 in case of an overflow.
    Next check of compression is enough to catch it. */

void ALPM_columntoi2_ce_se(CsvTable* table, POW10* pow, int** out_array, int** err_array, int column)
{
    double current_value = 0.0;
    double decompressed_value = 0.0;
 
    for (size_t i = 0; i < table->num_rows; i++)
    {
        current_value = atof(table->table[i][column - 1]);
        out_array[i][column - 1] = lrint(current_value * F10[pow->pos] * i_F10[pow->neg]);

        decompressed_value = out_array[i][column - 1] * F10[pow->neg] * i_F10[pow->pos];
        if (fabs(decompressed_value - current_value) > MAX_ERR)
        {
            err_array[i][column - 1] = 1;
            out_array[i][column - 1] = 0;
        }
        else err_array[i][column - 1] = 0;
    }
}

void ALPM_tabletoi2_ce_se(CsvTable* table, POW10* pow, int** out_array, int** err_array)
{
    double current_value = 0.0;
    double decompressed_value = 0.0;

    for (size_t i = 0; i < table->num_rows; i++)
    {
        for (size_t j = 0; j < table->num_fields; j++)
        {
            current_value = atof(table->table[i][j]);
            out_array[i][j] = lrint(current_value * F10[pow->pos] * i_F10[pow->neg]);

            decompressed_value = out_array[i][j] * F10[pow->neg] * i_F10[pow->pos];
            if (fabs(decompressed_value - current_value) > MAX_ERR)
            {
                err_array[i][j] = 1;
                out_array[i][j] = 0;
            }
            else err_array[i][j] = 0;
        }
    }
}

/* Functions for testing, there are no checks for compression and size errors */
void ALPM_columntoi2(CsvTable* table, POW10* pow, void** out_array, int column)
{
    for (size_t i = 0; i < table->num_rows; i++)
    {
        ((int**)out_array)[i][column - 1] = lrint(atof(table->table[i][column - 1]) * F10[pow->pos] * i_F10[pow->neg]);
    }
}

void ALPM_tabletoi2(CsvTable* table, POW10* pow, void** out_array)
{
    for (size_t i = 0; i < table->num_rows; i++)
    {
        for (size_t j = 0; j < table->num_fields; j++)
        {
            ((int**)out_array)[i][j] = lrint(atof(table->table[i][j]) * F10[pow->pos] * i_F10[pow->neg]);
        }
    }
}

/*  DELTA ENCODING */
/*  IDEA #1
    Search the mean of absolute values in current table -> pivot value.
    IDEA #2
    Calculate standart deviation of absolute values and density of probabilities.
    Search pivot value in an appropriate layer (determine via practice).
    IDEA #3
    Using all values, not only abs.
*/

static void delta_chginput(CsvTable* table, int** input, unsigned long delta, int column)
{
    if (column == NULL)
    {
        for (size_t i = 0; i < table->num_rows; i++)
        {
            for (size_t j = 0; j < table->num_fields; j++)
            {
                input[i][j] = input[i][j] - delta;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < table->num_rows; i++)
        {
            input[i][column - 1] = input[i][column - 1] - delta;
        }
    }
}

int delta_encode_ce(CsvTable* table, int** input, int** err, int options, int column, int of_r)
{
    unsigned long long count = 0;
    unsigned long mid_mean = 0;
    unsigned long delta = 0;
    int of_restrict = 0;

    switch (options)
    {
    case CONTINUOUS:
    {
        for (size_t i = 0; i < table->num_rows; i++)
        {
            for (size_t j = 0; j < table->num_fields; j++)
            {
                if (err[i][j] == 0)
                {
                    count += abs(input[i][j]);

                    of_restrict++;
                    if (of_restrict == of_r)
                    {
                        mid_mean = mid_mean + lrint((double)count / of_r);
                        of_restrict = 0;
                    }
                }
                else continue;
            }
        }
        if (of_restrict != 0)
            delta = mid_mean + lrint((double)count / of_restrict);
        else
            delta = mid_mean;

        delta_chginput(table, input, delta, NULL);
        return delta;
    }
    case ONE_COLUMN:
    {
        for (size_t i = 0; i < table->num_rows; i++)
        {
            if (err[i][column-1] == 0)
            {
                count += abs(input[i][column-1]);

                of_restrict++;
                if (of_restrict == of_r)
                {
                    mid_mean = mid_mean + lrint((double)count / of_r);
                    of_restrict = 0;
                }
            }
            else continue;
        }
        if (of_restrict != 0)
            delta = mid_mean + lrint((double)count / of_restrict);
        else
            delta = mid_mean;

        delta_chginput(table, input, delta, column);
        return delta;
    }
    }
}

/* Probably working worse, need comparison with previous fuction */
int delta_encode(CsvTable* table, int** input, int options, int column, int of_r)
{
    unsigned long long count = 0;
    unsigned long mid_mean = 0;
    unsigned long delta = 0;
    int of_restrict = 0;

    switch (options)
    {
    case CONTINUOUS:
    {
        for (size_t i = 0; i < table->num_rows; i++)
        {
            for (size_t j = 0; j < table->num_fields; j++)
            {
                count += abs(input[i][j]);

                of_restrict++;
                if (of_restrict == of_r)
                {
                    mid_mean = mid_mean + lrint((double)count / of_r);
                    of_restrict = 0;
                }
            }
        }
        if (of_restrict != 0)
            delta = mid_mean + lrint((double)count / of_restrict);
        else
            delta = mid_mean;       

        delta_chginput(table, input, delta, NULL);
        return delta;
    }
    case ONE_COLUMN:
    {
        for (size_t i = 0; i < table->num_rows; i++)
        {
            count += abs(input[i][column-1]);

            of_restrict++;
            if (of_restrict == of_r)
            {
                mid_mean = mid_mean + lrint((double)count / of_r);
                of_restrict = 0;
            }
        }
        if (of_restrict != 0)
            delta = mid_mean + lrint((double)count / of_restrict);
        else
            delta = mid_mean;

        delta_chginput(table, input, delta, column);
        return delta;
    }
    }
}

/* currently unused */

static double calculate_SD(double* data, size_t size)
{
    double sum = 0.0;
    double mean = 0.0;
    double SD = 0.0;

    for (int i = 0; i < size; i++)
        sum += data[i];

    mean = sum / size;

    for (int i = 0; i < size; i++)
        SD += pow(data[i] - mean, 2);

    return sqrt(SD / size);
}

static double calculate_D(double* data, size_t size)
{
    double sum = 0.0;
    double mean = 0.0;
    double D = 0.0;

    for (int i = 0; i < size; i++)
        sum += data[i];

    mean = sum / size;

    for (int i = 0; i < size; i++)
        D += pow(data[i] - mean, 2);

    return D / size;
}
