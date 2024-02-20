#include "ALPM.h"
#include <math.h>

struct _decimals
{
    unsigned int mean_p;
    unsigned int max_p;
    unsigned int min_p;
};

static DECIMALS get_decimals(char* str_r, size_t elements, char delim)
{
    unsigned int count = 0;
    unsigned int max = 0;
    unsigned int min = 0;
    unsigned int mean = 0;
    unsigned short after_dot = 0;
    size_t idx = 0;

    for (size_t i = 0; i < elements - 1; i++)
    {
        for (idx; str_r[idx] != delim; idx++)
        {
            if (str_r[idx] == '.' && !after_dot)
            {
                after_dot = 1;
                continue;
            }
            if (after_dot)
            {
                count++;
            }
        }
        max = count > max ? count : max;
        min = count > min ? min : count;
        count = 0;
        after_dot = 0;
        idx++;
    }
    for (idx; str_r[idx] != '\0'; idx++)
    {
        if (str_r[idx] == '.' && !after_dot)
        {
            after_dot = 1;
            continue;
        }
        if (after_dot)
        {
            count++;
        }
        max = count > max ? count : max;
        min = count > min ? min : count;
    }

    mean = lrint((double)(max + min) / 2);

    DECIMALS DEC = { mean, max, min };
    return DEC;
}

POW10 ALPM_calculatepow(char* str_r, size_t elements, int options, double* input_data)
{
    DECIMALS dec = get_decimals(str_r, elements, ',');
    switch (options)
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
