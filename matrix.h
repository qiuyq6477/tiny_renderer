#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include <stdbool.h>
#include "vector.h"

// 支持任意大小的二维矩阵
typedef struct {
    size_t rows;
    size_t cols;
    float* data; // 行优先存储
} matrix_t;

// 创建rows x cols的矩阵，所有元素初始化为0
matrix_t matrix_create(size_t rows, size_t cols);
// 释放矩阵内存
void matrix_free(matrix_t* mat);
// 获取(i, j)元素
float matrix_get(const matrix_t* mat, size_t i, size_t j);
// 设置(i, j)元素
void matrix_set(matrix_t* mat, size_t i, size_t j, float value);
// 单位矩阵
matrix_t matrix_identity(size_t n);
// 矩阵加法
matrix_t matrix_add(const matrix_t* a, const matrix_t* b);
// 矩阵乘法
matrix_t matrix_mul(const matrix_t* a, const matrix_t* b);
// 矩阵转置
matrix_t matrix_transpose(const matrix_t* mat);
// 判断两个矩阵是否相等
bool matrix_equal(const matrix_t* a, const matrix_t* b, float eps);
// 全零矩阵
matrix_t matrix_zeros(size_t rows, size_t cols);
// 全一矩阵
matrix_t matrix_ones(size_t rows, size_t cols);
// 对角矩阵
matrix_t matrix_diag(const float* diag, size_t n);
// 从一维数组初始化
matrix_t matrix_from_array(const float* arr, size_t rows, size_t cols);
// 支持C99风格二维数组初始化（如 float arr[2][3]）
matrix_t matrix_from_2darray(size_t rows, size_t cols, const float arr[rows][cols]);
// 3x3矩阵与vec3_t相乘
vec3_t matrix_mul_vec3(const matrix_t* mat, vec3_t v);

#endif // MATRIX_H 