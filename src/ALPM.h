#ifndef ALPM_H
#define ALPM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#define COMPRESS_EVERY_VALUE 100
#define BALANCED 101
#define BEST_ANALYZED 102
#define FIXED 103

typedef struct _decimals DECIMALS;

typedef struct _pow POW10;
struct _pow
{
    unsigned short neg;
    unsigned short pos;
};

POW10 ALPM_calculatepow(char* str_r, size_t elements, int options, double* input_data);



#ifdef __cplusplus
};

#endif
#endif
