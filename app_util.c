/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <string.h>
#include "wish_debug.h"
#include "app_util.h"
#include "bson.h"
#include "wish_fs.h"

int app_util_load_file(char* filename, bson* bs) {
    if (filename == NULL) {
        return APP_UTIL_ERR_FILE_NOT_FOUND;
    }
    size_t fn_len = strnlen(filename, APP_UTIL_FILENAME_MAX_LEN);
    if (fn_len == 0 || fn_len >= APP_UTIL_FILENAME_MAX_LEN) {
        return APP_UTIL_ERR_FILE_NOT_FOUND;
    }
    
    wish_file_t fd = wish_fs_open(filename);
    if (fd <= 0) {
        WISHDEBUG(LOG_CRITICAL, "Error opening file %s, %i", filename, fd);
        return APP_UTIL_ERR_FILE_NOT_FOUND;
    }
    
    wish_fs_lseek(fd, 0, WISH_FS_SEEK_SET);

    /* Size of the BSON structure in the file */
    int size = 0;
   
    /* Peek data size of BSON structure from file */
    int read_ret = wish_fs_read(fd, (void*) &size, 4);

    if (read_ret == 0 || (size <= 4 || size >= APP_UTIL_BSON_MAX_LEN)) {
        wish_fs_close(fd);
        return APP_UTIL_ERR_BSON_CORRUPTED;
    } 
    
    bson_init_size(bs, size);
    
    /* Go back to start and read the whole file to bson buffer */
    wish_fs_lseek(fd, 0, WISH_FS_SEEK_SET);
    read_ret = wish_fs_read(fd, ((void*)bs->data), size);
    
    if (read_ret != size) {
        WISHDEBUG(LOG_CRITICAL, "failed to read %i bytes, got %i.", size, read_ret);
        wish_fs_close(fd);
        return APP_UTIL_ERR_IO;
    }
    
    wish_fs_close(fd);
    
    return read_ret;
}

int app_util_save_file(char* filename, bson* bs) {
    if (filename == NULL) {
        return APP_UTIL_ERR_FILE_NOT_FOUND;
    }
    size_t fn_len = strnlen(filename, APP_UTIL_FILENAME_MAX_LEN);
    if (fn_len == 0 || fn_len >= APP_UTIL_FILENAME_MAX_LEN) {
        return APP_UTIL_ERR_FILE_NOT_FOUND;
    }
    
    wish_file_t fd = wish_fs_open(filename);
    if (fd <= 0) {
        WISHDEBUG(LOG_CRITICAL, "file does not exist... %s ", filename);
    }
    
    wish_fs_lseek(fd, 0, WISH_FS_SEEK_SET);
    int wr_ret = wish_fs_write(fd, bson_data(bs), bson_size(bs));
    wish_fs_close(fd);
    return wr_ret;
}
