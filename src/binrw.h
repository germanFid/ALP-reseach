#ifndef BINRW_H
#define BINRW_H

#ifdef __cplusplus
extern "C"
{
#endif

#define BINARY_READ "rb"
#define BINARY_WRITE "wb"

typedef struct _binrwhandle* BinRWHandle;

typedef struct _blocks BinBlocks;
struct _blocks
{
	unsigned int SZ_ELEMENT;
	unsigned int BLOCK_LENGTH;
	char ERROR_CODE;
};

/* Initialize blocks for writing to binary */
BinBlocks init_blocks(unsigned int SZ_ELEMENT, unsigned int BLOCK_LENGTH, char ERROR_CODE);

/* Create handle and open binary file */
BinRWHandle bin_fopenf(const char* filename, char* mode, BinBlocks* blocks);

/* Close handle, release resources */
BinRWHandle bin_close(BinRWHandle handle);

/* Write block to binary file */
void bin_writeb(BinRWHandle handle, char neg, char pos, void* ints, void* error, double* orig_data);

#ifdef __cplusplus
};

#endif
#endif
