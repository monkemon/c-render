#ifndef VEC_MAT_3x3_H
#define VEC_MAT_3x3_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "vec_mat_common.h"


/**
 * @brief Normalizes vector, returns pointer
 */
float* normalize_vec3(vec_3f v)
{
    float magnitude = sqrt(
        pow(v[0], 2.0f) + pow(v[1], 2.0f) + pow(v[2], 2.0f)
    );
    v[0] /= magnitude;
    v[1] /= magnitude;
    v[2] /= magnitude;

    return &v[0];
}

/**
 * @brief Subtracts A from B (result is B - A, stored in A)
 * 
 * @return pointer to result (same as A)
 */
float* subtract_vec3(vec_3f A, vec_3f B)
{
    A[0] -= B[0];
    A[1] -= B[1];
    A[2] -= B[2];
    return &A[0];
}

/**
 * @brief Cross product of A and B, stored in A, order is important
 * @return pointer to result (same as A)
 */
float* cross_vec3(vec_3f A, vec_3f B)
{
    vec_3f tmp;
    tmp[0] = (A[1] * B[2]) - (A[2] * B[1]);
    tmp[1] = (A[2] * B[0]) - (A[0] * B[2]);
    tmp[2] = (A[0] * B[1]) - (A[1] * B[0]);
    A[0] = tmp[0];
    A[1] = tmp[1];
    A[2] = tmp[2];
    return &A[0];
}


#endif // VEC_MAT_3x3_H