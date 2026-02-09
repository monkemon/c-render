#include "dynamic_buffer.h"

void bfr_init(s_dbuffer* buff, size_t element_size)
{
    bfr_init_c(buff, element_size, DYNAMIC_BUFFER_INITIAL_CAP);
}

void bfr_init_c(s_dbuffer* buff, size_t element_size, size_t initial_cap)
{
    assert(buff != NULL);
    assert(element_size > 0);

    buff->element_size = element_size;
    buff->size = 0;
    buff->capacity = initial_cap;
    buff->data = malloc(initial_cap * element_size);
}

void bfr_append(s_dbuffer* buff, void* element)
{
    assert(buff != NULL);
    assert(element != NULL);

    if (buff->size >= buff->capacity)
    {
        buff->capacity *= DYNAMIC_BUFFER_GROWTH_FACTOR;
        buff->data = realloc(buff->data, buff->capacity * buff->element_size);
    }

    void* dst = (char*)buff->data + (buff->size * buff->element_size); 
    memcpy(dst, element, buff->element_size);
    buff->size++;
}

void* bfr_get(s_dbuffer* buff, size_t idx)
{
    assert(buff != NULL);

    if (idx >= buff->size) return NULL;

    return (char*)buff->data + (idx * buff->element_size);
}

bool bfr_set(s_dbuffer* buff, size_t idx, void* element)
{
    assert(buff != NULL);

    if (idx > buff->size) return false;

    void* dest = (char*)buff->data + (idx * buff->element_size);
    memcpy(dest, element, buff->element_size);
    return true;
}

void bfr_clear(s_dbuffer* buff)
{
    assert(buff != NULL);

    buff->size = 0;
}


void bfr_free(s_dbuffer* buff)
{
    assert(buff != NULL);
    free(buff->data);
    buff->data = NULL;
    buff->size = 0;
    buff->capacity = 0;
}

void* bfr_raw(s_dbuffer* buff)
{
    assert(buff != NULL);
    return buff->data;
}