#ifndef VEC_MAT_COMMON_H
#define VEC_MAT_COMMON_H

#define PI 3.14159265358979323846
#define PI_PI_180 0.017453292519944444
#define PI_180_PI 57.29577951307855

#define RADS(x) ((x) * (PI / 180))

typedef float mat_4x4f[16];
typedef float vec_4f[4];

typedef float mat_3x3f[9];
typedef float vec_3f[3];

typedef enum 
{
    ax_x = 0,
    ax_y = 1,
    ax_z = 2
} e_axis;



#endif // VEC_MAT_COMMON_H