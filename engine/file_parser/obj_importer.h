/**
 * @file obj_importer.h
 * 
 * @brief parses data in .obj format
 * 
 * The vertex data is represented by four vertex lists; one for each type
 * of vertex coordinate. A right-hand coordinate system is used to specify
 * the coordinate locations.
 * 
 */

#ifndef OBJ_IMPORTER_H
#define OBJ_IMPORTER_H

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>


#include "file_loader.h"
#include "dynamic_buffer.h"


#define LINE_BUFF_SIZE 512

#define OBJ_COMMENT    "#"
#define OBJ_OBJNAME    "o"
#define OBJ_VERTEX     "v"
#define OBJ_NORMAL     "vn"
#define OBJ_TEXCOORD   "vt"
#define OBJ_PARAM      "vp"
#define OBJ_FACE       "f"
#define OBJ_MTLLIB     "mtllib"
#define OBJ_USEMTL     "usemtl"
#define OBJ_SMOOTH     "s"
#define OBJ_GROUP      "g"

typedef enum
{
    OBJ_MESH_VERT = 1<<0,
    OBJ_MESH_NORMAL = 1<<1,
    OBJ_MESH_TEXTURE = 1<<2
} e_obj_mesh;

typedef enum
{
    obj_face_v,
    obj_face_v_vt,
    obj_face_v_vn,
    obj_face_v_vt_vn,
    obj_face_undefined
} e_obj_face_type;

typedef enum
{
    obj_f_vert = 0,
    obj_f_norm = 1,
    obj_f_text = 2
} e_obj_idx_type;

typedef int face_def[3];

typedef struct 
{
    char* name;
    char* materiallib;
    char* material;

    int vertex_count;
    s_dbuffer vertexs;

    int normal_count;
    s_dbuffer normals;

    int text_coord_count;
    s_dbuffer textures;

    int face_data_count;
    s_dbuffer faces;
    e_obj_face_type face_type;
} s_obj_parsed_buff;

typedef struct
{
    float* data;
    int* indices;
    size_t vert_count;
    int stride;
} gpu_mesh_data;


bool obj_init_data_buffer(s_obj_parsed_buff* bfobj);
bool process_line(s_obj_parsed_buff* bfobj, char* line);
bool obj_parse(s_obj_parsed_buff* bfobj, s_file_buffer* fb);
e_file_result obj_parse_file(s_obj_parsed_buff* bfobj, char* f_name);
void obj_destroy_data_buffer(s_obj_parsed_buff* bfobj);

gpu_mesh_data* obj_cook_mesh_data(s_obj_parsed_buff* bfobj, uint32_t flags);
void destroy_mesh_data(gpu_mesh_data* d);

#endif // OBJ_IMPORTER_H