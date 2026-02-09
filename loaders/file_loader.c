#include "file_loader.h"

/**
 * @brief init file buffer structure
 * 
 * @param f_buffer file buffer structure pointer
 * 
 * @return result of operation
 */
e_file_result init_file_buffer(s_file_buffer* f_buffer, size_t max_size)
{
    f_buffer->data = (char*) calloc(max_size, sizeof(char));
    if (f_buffer->data == NULL)
    {
        fprintf(stderr, "calloc(): allocation of %ldB failed\r\n", max_size);
        return res_failed;
    }
    f_buffer->capacity = max_size;
    f_buffer->size = 0;

    return res_ok;
}

/**
 * @brief clears the file buffer without destroying
 * 
 * @param f_buffer file buffer structure pointer
 * 
 * @return result of operation
 */
e_file_result clear_file_buffer(s_file_buffer* f_buffer)
{
    if (f_buffer->data != NULL)
    {
        f_buffer->size = 0;
        fprintf(stdout, "Clearing buffer of capacity %ld\r\n", f_buffer->capacity);
    }
    return res_ok;
}


/**
 * @brief destroys the buffer, freeing memory and setting size and cap to 0
 * 
 * @param f_buffer file buffer structure pointer
 * 
 * @return result of operation
 */
e_file_result destroy_file_buffer(s_file_buffer* f_buffer)
{
    if (f_buffer->data != NULL)
    {
        free(f_buffer->data);
        f_buffer->data = NULL;

        fprintf(stdout, "Freed %ld B, was using %ld\r\n", f_buffer->capacity, f_buffer->size);
    }
    f_buffer->size = 0;
    f_buffer->capacity = 0;

    return res_ok;
}


/**
 * @brief get file size using stat. If error, the function returns -1
 * 
 * @param filename path to the file
 * 
 * @return int: size of the file
 */
long get_file_size(char* filename)
{
    struct stat st;
    int res = stat(filename, &st);
    if (res != 0)
    {
        return -1;
    }
    return st.st_size;
}

/**
 * @brief Loads contents of file into s_file_buffer structure, expects preallocated buffer, null-terminates
 * 
 * @param f_buffer - buffer to populate
 * @param f_path - file path
 * @param f_len - how many bytes to read from file
 * 
 * @return result of operation
 * 
 * @todo In the future consider iterating over FILE* instead of loading everything into ram
 */
e_file_result read_file(s_file_buffer* f_buffer, char* f_path, size_t f_len)
{
    size_t read_bytes = 0;
    FILE* f_ptr = NULL;

    if (f_buffer->data == NULL)
    {
        fprintf(stderr, "Buffer not initialized for file [%s]\r\n", f_path);
        return res_failed;
    }

    if (f_buffer->capacity < f_len)
    {
        fprintf(
            stderr,
            "Buffer not big enough for file [%s] (need: %zu, has: %zu)\r\n",
            f_path,
            f_len,
            f_buffer->capacity
        );
        return res_buffer_too_small;
    }

    f_ptr = fopen(f_path, "rb");
    if (!f_ptr)
    {
        perror("fopen");
        return res_failed;
    }

    read_bytes = fread(f_buffer->data, sizeof(char), f_len, f_ptr);
    if (read_bytes != f_len)
    {
        fprintf(stderr, "fread() read bytes mismatch - [%s], expected: %ld, read: %zu\r\n", f_path, f_len, read_bytes);
        f_buffer->size = 0;
        fclose(f_ptr);
        return res_failed;
    }

    fprintf(stdout, "fread() - read %zu bytes from [%s]\r\n", read_bytes, f_path);

    f_buffer->data[read_bytes] = 0;
    f_buffer->size = read_bytes + 1;

    fclose(f_ptr);

    return res_ok;
}


/**
 * @brief initializes file buffer object and reads file into it
 * 
 * @param f_buffer pointer to file buffer structure
 * @param f_path path to file
 * 
 * @return result of the operation
 */
e_file_result load_init_file(s_file_buffer* f_buffer, char* f_path)
{
    long file_size = -1;

    if ((file_size = get_file_size(f_path)) < 0)
    {
        fprintf(stderr, "load_init_file(): failed to get file size for [%s]\r\n", f_path);
        return res_failed;
    }

    if (init_file_buffer(f_buffer, (size_t)file_size + 1) != res_ok)
    {
        fprintf(stderr, "load_init_file(): failed to init buffer\r\n");
        return res_failed;
    }

    if (read_file(f_buffer, f_path, file_size) != res_ok)
    {
        fprintf(stderr, "load_init_file(): failed to read file\r\n");
        destroy_file_buffer(f_buffer);
        return res_failed;
    }

    return res_ok;
}