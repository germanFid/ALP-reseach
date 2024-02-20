#include "binrw.h"
#include "ALPM.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct _binrwhandle
{
	FILE* fp;
	BinBlocks blocks;
};

BinBlocks init_blocks(unsigned int SZ_ELEMENT, unsigned int BLOCK_LENGTH, char ERROR_CODE)
{
	BinBlocks blocks = { SZ_ELEMENT, BLOCK_LENGTH, ERROR_CODE };
	return blocks;
}

BinRWHandle bin_fopenf(const char* filename, char* mode, BinBlocks* blocks)
{
	BinRWHandle handle = calloc(1, sizeof(struct _binrwhandle));
	if (!handle)
		return NULL;

	handle->fp = fopen(filename, mode);
	if (!handle->fp)
	{
		free(handle);
		return NULL;
	}

	handle->blocks = *blocks;

	return handle;
}

BinRWHandle bin_close(BinRWHandle handle)
{
	fclose(handle->fp);
	free(handle);
}

void bin_writeb(BinRWHandle handle, char neg, char pos, void* ints, void* error, double* orig_data)
{
	/* No-error code = 0 */
	char no_err = 0b0;
	unsigned int count_err = 0;

	/* Write 2 bytes of powers of 10 */
	fwrite(&neg, 1, 1, handle->fp);
	fwrite(&pos, 1, 1, handle->fp);

	/* Write 4 bytes of block length */
	fwrite(&handle->blocks.BLOCK_LENGTH, 4, 1, handle->fp);

	/* Write elements */
	if (handle->blocks.SZ_ELEMENT == sizeof(int))
	{
		for (unsigned int i = 0; i < handle->blocks.BLOCK_LENGTH; i++)
		{
			if (((int*)error)[i])
			{
				/* If error -> write error code and move to next element */
				fwrite(&handle->blocks.ERROR_CODE, 1, 1, handle->fp);
				count_err++;
			}
			else
			{
				/* If no error -> write element */
				fwrite(&no_err, 1, 1, handle->fp);
				fwrite(&((int*)ints)[i], handle->blocks.SZ_ELEMENT, 1, handle->fp);
			}
		}
	}
	else if (handle->blocks.SZ_ELEMENT == sizeof(long long))
	{
		for (unsigned int i = 0; i < handle->blocks.BLOCK_LENGTH; i++)
		{
			if (((int*)error)[i])
			{
				/* If error -> write error code and move to next element */
				fwrite(&handle->blocks.ERROR_CODE, 1, 1, handle->fp);
				count_err++;
			}
			else
			{
				/* If no error -> write element */
				fwrite(&no_err, 1, 1, handle->fp);
				fwrite(&((long long*)ints)[i], handle->blocks.SZ_ELEMENT, 1, handle->fp);
			}
		}
	}

	/* Write remained doubles corresponding to error places */
	if (count_err > 0)
	{
		for (unsigned int i = 0; i < handle->blocks.BLOCK_LENGTH; i++)
		{
			if (((int*)error)[i])
			{
				fwrite(&orig_data[i], sizeof(double), 1, handle->fp);
			}
		}
	}
}

void bin_readb(BinRWHandle handle, char* neg, char* pos, void* ints, void* error, double* orig_data)
{
	/* No-error code = 0 */
	char err = 0b0;
	unsigned int count_err = 0;

	/* Read 2 bytes of powers of 10*/
	fread(neg, 1, 1, handle->fp);
	fread(pos, 1, 1, handle->fp);

	/* Read 4 bytes of block length */
	fread(&handle->blocks.BLOCK_LENGTH, 4, 1, handle->fp);

	size_t read_chunks = fread(&err, 1, 1, handle->fp);
	size_t read_el = 0;
	while (read_chunks != 0)
	{
		if (err)
		{
			count_err++;
		}

		else
		{
			/* Process element */
						fread(&err, 1, 1, handle->fp);
			read_chunks = fread(&((int*)ints)[read_el],
			   handle->blocks.SZ_ELEMENT, 1, handle->fp);

			int first_byte = (((int*)ints)[read_el] >> 0) & 0xff;

			if (first_byte == 0)
			{
				break; // or what?
			}

			read_el++;
		}
	}
	
}
