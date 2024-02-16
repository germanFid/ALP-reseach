/*  =========================================
    == fast CSV reader with memory mapping ==
    =========================================

 * 1. open CSV file by calling csv_fopend("filename.csv")
 * 2. read CSV row by calling csv_readrow(handle)
 * 3. read single CSV field by calling csv_readfield(returned_row, handle)
*/

#ifndef CSV_H
#define CSV_H

#ifdef __cplusplus
extern "C" 
{  
#endif

/* pointer to private handle structure */
typedef struct _csvhandle *CsvHandle;

/*
 * openes csv file
 * @filename: pathname of the file
 * @return: csv handle
 * csv_close() should be called to release resources
 */
CsvHandle csv_fopend(const char* filename);
CsvHandle csv_fopenf(const char* filename, char delim, char quote, char escape);

/*
 * closes csv handle, releasing all resources
 * @handle: csv handle
 */
void csv_close(CsvHandle handle);

/*
 * reads (first / next) line of csv file
 * @handle: csv handle
 */
char* csv_readrow(CsvHandle handle);

/*
 * get (first / next) field of csv row
 * @row: csv row (you can use csv_readrow() to parse next line)
 * @handle: csv handle
 */
char* csv_readfield(char* row, CsvHandle handle);

/*
 * get numbers of fields in csv file / row
 * @row: csv row (you can use csv_readrow() to parse next line)
 * @handle: csv handle
 */
int csv_getnumfields(char* row, CsvHandle handle);

#ifdef __cplusplus
};

#endif
#endif
