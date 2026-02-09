#define _POSIX_C_SOURCE 200809L
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "vec_mat_4x4.h"
#include "obj_importer.h"


void printline(void)
{
    printf("\r\n~~~~~~~~~~~~~~~~~~~~~~\r\n");
    return;
}



int main(int argc, char** args)
{
    s_obj_parsed_buff data;

    obj_parse_file(&data, "./assets/objects/basic_cube.obj");

    printf("type: %d\n", data.face_type);

    obj_destroy_data_buffer(&data);
}