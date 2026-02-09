#include "obj_importer.h"

bool obj_init_data_buffer(s_obj_parsed_buff* bfobj)
{
    bfobj->name = NULL;
    bfobj->material = NULL;
    bfobj->materiallib = NULL;
    bfobj->face_type = obj_face_undefined;
    bfr_init(&bfobj->vertexs, sizeof(float));
    bfr_init(&bfobj->normals, sizeof(float));
    bfr_init(&bfobj->textures, sizeof(float));
    bfr_init(&bfobj->faces, sizeof(face_def));
    return true;
}

void set_type(s_obj_parsed_buff* bfobj, e_obj_face_type t)
{
    if (bfobj->face_type == obj_face_undefined)
    {
        bfobj->face_type = t;
    }
}

bool process_line(s_obj_parsed_buff* bfobj, char* line)
{
    if (line[0] == '#') return true;
    if (line[0] == 0 || line[0] == '\n') return false;

    const char* delim = " ";
    char* saveptr = NULL;
    char* token = NULL;
    
    token = strtok_r(line, delim, &saveptr);

    // o Name
    if (!strncmp(token, OBJ_OBJNAME, 1))
    {
        char* data = strtok_r(NULL, delim, &saveptr);
        size_t len = strlen(data);
        bfobj->name = (char*) malloc(sizeof(char) * len + 1);
        memcpy(bfobj->name, data, len);
        bfobj->name[len] = 0;
        return true;
    }

    // vp u v (w=1.0)
    else if (!strncmp(token, OBJ_PARAM, 2))
    {
        printf("NOT IMPLEMENTED\n");
    }

    // vn i j k
    else if (!strncmp(token, OBJ_NORMAL, 2))
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            token = strtok_r(NULL, delim, &saveptr);
            if (token == NULL) break;
            float normal = atof(token);
            bfr_append(&bfobj->normals, &normal);
        }
        bfobj->normal_count += 3;
        return true;
    }

    // vt u (v=0) (w=0)
    else if (!strncmp(token, OBJ_TEXCOORD, 2))
    {
        for (uint8_t i = 0; i < 2; i++)
        {
            token = strtok_r(NULL, delim, &saveptr);
            if (token == NULL) break;
            float texcoord = atof(token);
            bfr_append(&bfobj->textures, &texcoord);
        }
        bfobj->text_coord_count += 2;
        return true;
    }

    // v x y z (w=1.0)
    else if (!strncmp(token, OBJ_VERTEX, 1))
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            float vert;
            token = strtok_r(NULL, delim, &saveptr);
            if (token == NULL) break;
            vert = atof(token);
            bfr_append(&bfobj->vertexs, &vert);
        }
        bfobj->vertex_count += 3;
        return true;
    }

    else if (!strncmp(token, OBJ_FACE, 1))
    {
        bool empty_texture = false;
        int slash_count = 0;

        int face_vert_count = 0;
        face_def faces[4];

        // token might look like this here `1/2/3` or `23//1` or `1/2`
        token = strtok_r(NULL, delim, &saveptr);

        for (char* p = token; *p != 0; p++)
        {
            if (*p == '/')
            {
                slash_count++;
                if (*(p+1) == '/') empty_texture = true;
            }
        }

        do
        {
            if (slash_count == 0)
            {
                faces[face_vert_count][obj_f_vert] = atoi(token);
                set_type(bfobj, obj_face_v);
            }
            else if (slash_count == 1)
            {
                char* slash_saveptr = NULL;
                char* v_str = strtok_r(token, "/", &slash_saveptr);
                char* vt_str = strtok_r(NULL, "/", &slash_saveptr);

                faces[face_vert_count][obj_f_vert] = atoi(v_str);
                faces[face_vert_count][obj_f_text] = atoi(vt_str);
                set_type(bfobj, obj_face_v_vt);
            }
            else if (slash_count == 2)
            {
                char* slash_saveptr = NULL;
                char* v_str = strtok_r(token, "/", &slash_saveptr);
                char* vt_str = strtok_r(NULL, "/", &slash_saveptr);
                char* vn_str = strtok_r(NULL, "/", &slash_saveptr);

                faces[face_vert_count][obj_f_vert] = atoi(v_str);
                if (empty_texture)
                {
                    faces[face_vert_count][obj_f_norm] = atoi(vt_str);
                    set_type(bfobj, obj_face_v_vn);
                }
                else
                {
                    faces[face_vert_count][obj_f_text] = atoi(vt_str);
                    faces[face_vert_count][obj_f_norm] = atoi(vn_str);
                    set_type(bfobj, obj_face_v_vt_vn);
                }
            }

            face_vert_count++;

        } while ( (token = strtok_r(NULL, delim, &saveptr)) != NULL );

        if (face_vert_count == 3)
        {
            bfr_append(&bfobj->faces, faces[0]);
            bfr_append(&bfobj->faces, faces[1]);
            bfr_append(&bfobj->faces, faces[2]);
            bfobj->face_data_count += 3;
        }
        else if (face_vert_count == 4)
        {
            bfr_append(&bfobj->faces, faces[0]);
            bfr_append(&bfobj->faces, faces[1]);
            bfr_append(&bfobj->faces, faces[2]);
            bfr_append(&bfobj->faces, faces[0]);
            bfr_append(&bfobj->faces, faces[2]);
            bfr_append(&bfobj->faces, faces[3]);
            bfobj->face_data_count += 6;
        }
        
        return true;
    }

    else if (!strncmp(token, OBJ_SMOOTH, 6))
    {

        printf("smooth\n");
    }

    else if (!strncmp(token, OBJ_GROUP, 6))
    {

        printf("group\n");
    }

    else if (!strncmp(token, OBJ_MTLLIB, 6))
    {
        char* data = strtok_r(NULL, delim, &saveptr);
        if (data)
        {
            size_t len = strlen(data);
            bfobj->materiallib = (char*)malloc(sizeof(char) * len + 1);
            memcpy(bfobj->materiallib, data, len);
            bfobj->materiallib[len] = 0;
        }
        return true;
    }

    else if (!strncmp(token, OBJ_USEMTL, 6))
    {
        char* data = strtok_r(NULL, delim, &saveptr);
        if (data)
        {
            size_t len = strlen(data);
            bfobj->material = (char*)malloc(sizeof(char) * len + 1);
            memcpy(bfobj->material, data, len);
            bfobj->material[len] = 0;
        }
        return true;
    }

    return false;
}

bool obj_parse(s_obj_parsed_buff* bfobj, s_file_buffer* fb)
{
    const char* line_delim = "\n";
    char* line_saveptr = NULL;
    char* line_token = NULL;
    bool res = true;

    line_token = strtok_r(fb->data, line_delim, &line_saveptr);

    while (line_token != NULL)
    {
        res = process_line(bfobj, line_token);

        line_token = strtok_r(NULL, line_delim, &line_saveptr);
    }

    return res;
}


// load file, send to obj_parse
e_file_result obj_parse_file(s_obj_parsed_buff* bfobj, char* f_name)
{
    //obj_destroy_data_buffer(bfobj);
    obj_init_data_buffer(bfobj);

    s_file_buffer f_buff;
    if (load_init_file(&f_buff, f_name) != res_ok)
    {
        fprintf(stderr, "Failed to load .obj file at [%s]\r\n", f_name);
        return res_failed;
    }

    obj_parse(bfobj, &f_buff);

    destroy_file_buffer(&f_buff);
    return res_ok;
}

void obj_destroy_data_buffer(s_obj_parsed_buff* bfobj)
{
    if (bfobj->name) free(bfobj->name);
    if (bfobj->material) free(bfobj->material);
    if (bfobj->materiallib) free(bfobj->materiallib);

    bfr_free(&bfobj->vertexs);
    bfr_free(&bfobj->normals);
    bfr_free(&bfobj->textures);
    bfr_free(&bfobj->faces);
}

gpu_mesh_data* obj_cook_mesh_data(s_obj_parsed_buff* bfobj, uint32_t flags)
{
    gpu_mesh_data* mesh = malloc(sizeof(gpu_mesh_data));
    mesh->data = NULL;
    mesh->indices = NULL;
    mesh->stride = 0;
    mesh->vert_count = 0;

    bool with_normals = (flags & OBJ_MESH_NORMAL) == OBJ_MESH_NORMAL;
    bool with_uvs = (flags & OBJ_MESH_TEXTURE) == OBJ_MESH_TEXTURE;

    int stride = 3;
    if (with_normals)
    {
        // can't fill normals, when no normals provided in file
        if (bfobj->face_type != obj_face_v_vn && bfobj->face_type != obj_face_v_vt_vn)
        {
            free(mesh);
            return NULL;
        }
        stride += 3;
    }

    if (with_uvs)
    {
        // can't fill uvs , when no uvs provided in file
        if (bfobj->face_type != obj_face_v_vt && bfobj->face_type != obj_face_v_vt_vn)
        {
            free(mesh);
            return NULL;
        }
        stride += 2;
    }

    mesh->stride = stride * sizeof(float);
    mesh->vert_count = bfobj->face_data_count; // triangulated (hopefully)
    mesh->indices = malloc(sizeof(int) * mesh->vert_count);
    mesh->data = malloc(sizeof(float) * mesh->vert_count * stride);

    face_def* face_ptr = (face_def*)bfobj->faces.data;
    float* vert_ptr = (float*)bfobj->vertexs.data;
    float* norm_ptr = (float*)bfobj->normals.data;
    float* uv_ptr = (float*)bfobj->textures.data;

    int write_idx = 0;
    int idx_idx = 0;

    for (size_t i = 0; i < mesh->vert_count; i++)
    {
        int vert_idx = face_ptr[i][obj_f_vert] - 1;
        int norm_idx = face_ptr[i][obj_f_norm] - 1;
        int uv_idx = face_ptr[i][obj_f_text] - 1;

        mesh->data[write_idx++] = vert_ptr[vert_idx*3 + 0];
        mesh->data[write_idx++] = vert_ptr[vert_idx*3 + 1];
        mesh->data[write_idx++] = vert_ptr[vert_idx*3 + 2];
        mesh->indices[idx_idx++] = face_ptr[i][obj_f_vert];

        if (with_normals)
        {
            mesh->data[write_idx++] = norm_ptr[norm_idx*3 + 0];
            mesh->data[write_idx++] = norm_ptr[norm_idx*3 + 1];
            mesh->data[write_idx++] = norm_ptr[norm_idx*3 + 2];
        }

        if (with_uvs)
        {
            mesh->data[write_idx++] = uv_ptr[uv_idx*2 + 0];
            mesh->data[write_idx++] = uv_ptr[uv_idx*2 + 1];
        }
    }

    return mesh;
}

void destroy_mesh_data(gpu_mesh_data* d)
{
    d->stride = 0;
    d->vert_count = 0;
    if (!d) return;
    if (d->data) free(d->data);
    if (d->indices) free(d->indices);
    free(d);
}