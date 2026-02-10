#ifndef VEC_MAT_4x4_H
#define VEC_MAT_4x4_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "vec_mat_common.h"


/**
 * @brief prints vector to stdout
 * 
 * @param v vector to be printed
 */
void print_vec_4f(vec_4f v)
{
    uint8_t idx = 0;

    fprintf(stdout, "Vector 4x1 float:\r\n");

    do
    {
        fprintf(stdout, "%0.3f\r\n", v[idx]);
        idx++;
    } 
    while (idx < 4);
}

/**
 * @brief prints matrix to stdout
 * 
 * @param m matrix to be printed
 */
void print_mat_4x4f(mat_4x4f m)
{
    uint8_t idx = 0;

    fprintf(stdout, "Matrix 4x4 float:\r\n");

    while (idx < 16)
    {
        if (idx != 0 && idx%4 == 0)
        {
            fprintf(stdout, "\r\n");
        }
        fprintf(stdout, "%0.3f\t", m[idx]);
        idx++;
    }
    fprintf(stdout, "\r\n");
}

/**
 * @brief sets matrix m to be identity matrix
 * 
 * @param m matrix to be set as identity
 */
void identity_mat_4x4f(mat_4x4f m)
{
    memset(m, 0, sizeof(mat_4x4f));
    m[0] = 1;
    m[5] = 1;
    m[10] = 1;
    m[15] = 1;
}


void ortho_projection_mat_4x4f(mat_4x4f m, float l, float r, float b, float t, float near, float far)
{
    m[0] = 2.0f / (r - l);
    m[3] = -((r + l) / (r - l));

    m[5] = 2 / (t-b);
    m[7] = -((t+b) / (t-b));

    m[10] = -2 / (far-near);
    m[11] = -((far+near) / (far-near));
}


void perspective_projection_mat_4x4f(mat_4x4f m, float fov_rads, float near, float far, float aspect)
{
    float tangent = tanf(fov_rads/2);
    float top = near * tangent;
    float right = top*aspect;

    m[0] = near / right;
    m[5] = near / top;
    m[10] = -(far + near) / (far - near);
    m[11] = -1;
    m[14] = -(2 * near * far) / (far-near);
    m[15] = 0;
}


/**
 * @brief multiply matrix m with scalar s
 * 
 * @param m matrix to be multiplied (Result stored here)
 * @param s scalar with which to multiply the matrix
 */
void scalar_mult_mat_4x4f(mat_4x4f m, float s)
{
    uint8_t idx = 0;
    do
    {
        m[idx] *= s;
        idx++;
    }
    while (idx < 16);
}


/**
 * @brief add m2 to m1. Result will be stored in m1
 * 
 * @param m1 first matrix (destination)
 * @param m2 second matrix 
 */
void add_mat_4x4f_mat_4x4f(mat_4x4f m1, mat_4x4f m2)
{
    uint8_t idx = 0;
    do
    {
        m1[idx] += m2[idx];
        idx++;
    }
    while (idx < 16);
}


/**
 * @brief multiply m1 with m2, store result into dest
 * 
 * @param m1 first matrix
 * @param m2 second matrix 
 * 
 * @param dest destination matrix
 */
void multiply_mat_4x4f_mat_4x4f(mat_4x4f m1, mat_4x4f m2, mat_4x4f dest)
{
    for (uint8_t r = 0; r < 4 ; r++)
    {
        for (uint8_t c = 0; c < 4 ; c++)
        {
            float res = 0;
            for (uint8_t i = 0; i < 4 ; i++)
            {
                res += m1[(r*4) + i] * m2[(i*4) + c];
            }
            dest[(r*4) + c] = res;
        }
    }
}

/**
 * @brief multiply m1 with m2, store result into dest
 * 
 * @param m matrix
 * @param v vector
 * @param dest vector where the result will be stored
 */
void multiply_vec_4f_mat_4x4f(mat_4x4f m, vec_4f v, vec_4f dest)
{
    for (uint8_t r = 0; r < 4; r++)
    {
        float res = 0;
        for (uint8_t i = 0; i < 4; i++)
        {
            res += m[(r*4) + i] * v[i];
        }
        dest[r] = res;
    }
}


/**
 * @brief set scaling components to the matrix
 * 
 * @param m input matrix
 * @param vs scaling vector
 * @param dest matrix to where to set the scaling componenets
 */
void set_scale_mat_4x4f(mat_4x4f m, vec_4f v, mat_4x4f dest)
{
    dest[0] = m[0] * v[0];
    dest[5] = m[5] * v[1];
    dest[10] = m[10] * v[2];
    dest[15] = m[15] * v[3];
}


/**
 * @brief set translation componenets to matrix
 * 
 * @param m input matrix
 * @param vt translation amount
 * @param dest matrix to where to set the translationn componenets
 */
void set_translate_mat_4x4f(mat_4x4f m, vec_4f vt, mat_4x4f dest)
{
    dest[3] = m[3] + vt[0];
    dest[7] = m[7] + vt[1];
    dest[11] = m[11] + vt[2];
}

/**
 * @brief rotate matrix around axis
 * 
 * @param m input matrix
 * @param angle_rads angle in radians
 * @param ax axis of rotation (enum e_axis)
 * 
 * @return float* to the rotated matrix
 */
float* rotate_mat_4x4f(mat_4x4f m, float angle_rads, e_axis ax)
{
    mat_4x4f rot;
    mat_4x4f temp;

    identity_mat_4x4f(rot);

    switch (ax)
    {
    case ax_x:
            rot[5] =  cosf(angle_rads);
            rot[6] = -sinf(angle_rads);
            rot[9] =  sinf(angle_rads);
            rot[10]=  cosf(angle_rads);
        break;

    case ax_y:
            rot[0] =  cosf(angle_rads);
            rot[2] =  sinf(angle_rads);
            rot[8] = -sinf(angle_rads);
            rot[10]=  cosf(angle_rads);
        break;

    case ax_z:
            rot[0] =  cosf(angle_rads);
            rot[1] = -sinf(angle_rads);
            rot[4] =  sinf(angle_rads);
            rot[5] =  cosf(angle_rads);
        break;

    default:
        break;
    }

    multiply_mat_4x4f_mat_4x4f(m, rot, temp);
    memcpy(m, temp, sizeof(mat_4x4f));

    return m;
}

/**
 * @brief scale matrix
 * 
 * @param m input matrix
 * @param scale_vec scaling factors (x, y, z)
 * 
 * @return float* of resulting matrix
 */
float* scale_mat_4x4f(mat_4x4f m, vec_4f scale_vec)
{
    mat_4x4f scale;
    mat_4x4f temp;

    identity_mat_4x4f(scale);
    
    scale[0] = scale_vec[0];
    scale[5] = scale_vec[1];
    scale[10] = scale_vec[2];
    
    multiply_mat_4x4f_mat_4x4f(m, scale, temp);
    memcpy(m, temp, sizeof(mat_4x4f));

    return m;
}

/**
 * @brief translate matrix
 * 
 * @param m input matrix
 * @param trans_vec translation vector
 * 
 * @return float* of resulting matrix
 */
float* translate_mat_4x4f(mat_4x4f m, vec_4f trans_vec)
{
    mat_4x4f trans;
    mat_4x4f temp;

    identity_mat_4x4f(trans);
    
    trans[3] = trans_vec[0];
    trans[7] = trans_vec[1];
    trans[11] = trans_vec[2];
    
    multiply_mat_4x4f_mat_4x4f(m, trans, temp);
    memcpy(m, temp, sizeof(mat_4x4f));

    return m;
}

float* look_at_mat_4x4f(mat_4x4f m, vec_3f pos, vec_3f dir, vec_3f right, vec_3f up)
{
    mat_4x4f tmp_rud;
    mat_4x4f tmp_pos;

    identity_mat_4x4f(tmp_rud);
    identity_mat_4x4f(tmp_pos);

    tmp_rud[0] = right[0];
    tmp_rud[1] = right[1];
    tmp_rud[2] = right[2];
    tmp_rud[4] = up[0];
    tmp_rud[5] = up[1];
    tmp_rud[6] = up[2];
    tmp_rud[8] = dir[0];
    tmp_rud[9] = dir[1];
    tmp_rud[10] = dir[2];

    tmp_pos[3] = -pos[0];
    tmp_pos[7] = -pos[1];
    tmp_pos[11] = -pos[2];

    multiply_mat_4x4f_mat_4x4f(tmp_rud, tmp_pos, m);
}

#endif // VEC_MAT_4x4_H