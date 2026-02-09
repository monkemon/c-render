/* file_loader.h */

#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>

typedef enum
{
    res_ok,
    res_failed,
    res_buffer_too_small
} e_file_result;

typedef struct
{
    char* data;
    size_t size;
    size_t capacity;
} s_file_buffer;

long get_file_size(char* filename);

e_file_result init_file_buffer(s_file_buffer* f_buffer, size_t max_size);
e_file_result clear_file_buffer(s_file_buffer* f_buffer);
e_file_result destroy_file_buffer(s_file_buffer* f_buffer);
e_file_result read_file(s_file_buffer* f_buffer, char* f_path, size_t f_len);
e_file_result load_init_file(s_file_buffer* f_buffer, char* f_path);

#endif //FILE_LOADER_H