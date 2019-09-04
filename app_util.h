#pragma once

#include "bson.h"
#include "bson_visit.h"

#define APP_UTIL_ERR_FILE_EMPTY 0
#define APP_UTIL_ERR_FILE_NOT_FOUND -1
#define APP_UTIL_ERR_BSON_CORRUPTED -2
#define APP_UTIL_ERR_IO -3

#define APP_UTIL_FILENAME_MAX_LEN 25
#define APP_UTIL_BSON_MAX_LEN 10*1024

/**
 * Load a BSON structure from a file. A buffer of adequate size for the BSON structure will be loaded from the heap, and it is the responsibility of the caller to free the memory with bsob_destroy()
 * @param filename
 * @param target
 * @return a positive value for a successful load, 0 or negative value for errors
 */
int app_util_load_file(char* filename, bson* target);

/**
 * Save BSON structure to a file. If the file exists, it will be deleted first.
 * @param filename
 * @param source
 * @return a positive value for a successful save, 0 or negative value for errors
 */
int app_util_save_file(char* filename, bson* source);
