#include "ALPM.h"
#include <stdint.h>

POW10 ALPM_calculatepow(char* str_r, size_t elements, int options)
{
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
        int max_precision = get_maxprecision(str_r, elements, ',');
        int pow_neg = (max_precision + 6) <= 10 ? (max_precision + 6) : 10;
        int pow_pos = pow_neg + max_precision;
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
        /* TODO: basically the same, but use mean, min and max precision */
    {
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

static int get_maxprecision(char* str_r, size_t elements, char delim)
{
    unsigned int cnt = 0;
    unsigned int res = 0;
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
                cnt++;
            }
        }
        res = cnt > res ? cnt : res;
        cnt = 0;
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
            cnt++;
        }
        res = cnt > res ? cnt : res;
    }

    return res;
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

static double get_mprecision(char* str_r, size_t elements, char delim)
{
    unsigned int cnt = 0;
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
                cnt++;
            }
        }
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
            cnt++;
        }
    }

    return (double)cnt / (double)elements;
}