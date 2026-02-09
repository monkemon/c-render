#ifndef DYNAMIC_BUFFER_H
#define DYNAMIC_BUFFER_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define DYNAMIC_BUFFER_INITIAL_CAP 16
#define DYNAMIC_BUFFER_GROWTH_FACTOR 2

typedef struct
{
    void* data;
    size_t size;
    size_t capacity;
    size_t element_size;
} s_dbuffer;


void bfr_init(s_dbuffer* buff, size_t element_size);
void bfr_init_c(s_dbuffer* buff, size_t element_size, size_t initial_cap);
void bfr_append(s_dbuffer* buff, void* element);
void* bfr_get(s_dbuffer* buff, size_t idx);
bool bfr_set(s_dbuffer* buff, size_t idx, void* element);
void bfr_clear(s_dbuffer* buff);
void bfr_free(s_dbuffer* buff);
void* bfr_raw(s_dbuffer* buff);


#endif