#include "matrix.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

matrix_t matrix_create(size_t rows, size_t cols) {
    matrix_t mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.data = (float*)calloc(rows * cols, sizeof(float));
    return mat;
}

void matrix_free(matrix_t* mat) {
    if (mat && mat->data) {
        free(mat->data);
        mat->data = NULL;
        mat->rows = 0;
        mat->cols = 0;
    }
}

float matrix_get(const matrix_t* mat, size_t i, size_t j) {
    return mat->data[i * mat->cols + j];
}

void matrix_set(matrix_t* mat, size_t i, size_t j, float value) {
    mat->data[i * mat->cols + j] = value;
}

matrix_t matrix_identity(size_t n) {
    matrix_t mat = matrix_create(n, n);
    for (size_t i = 0; i < n; ++i) {
        matrix_set(&mat, i, i, 1.0f);
    }
    return mat;
}

matrix_t matrix_add(matrix_t a, matrix_t b) {
    matrix_t result = matrix_create(a.rows, a.cols);
    if (a.rows != b.rows || a.cols != b.cols) return result;
    for (size_t i = 0; i < a.rows; ++i) {
        for (size_t j = 0; j < a.cols; ++j) {
            float v = matrix_get(&a, i, j) + matrix_get(&b, i, j);
            matrix_set(&result, i, j, v);
        }
    }
    return result;
}

matrix_t matrix_mul(matrix_t a, matrix_t b) {
    if (a.cols != b.rows) return matrix_create(0, 0);
    matrix_t result = matrix_create(a.rows, b.cols);
    for (size_t i = 0; i < a.rows; ++i) {
        for (size_t j = 0; j < b.cols; ++j) {
            float sum = 0.0f;
            for (size_t k = 0; k < a.cols; ++k) {
                sum += matrix_get(&a, i, k) * matrix_get(&b, k, j);
            }
            matrix_set(&result, i, j, sum);
        }
    }
    return result;
}

matrix_t matrix_transpose(matrix_t mat) {
    matrix_t result = matrix_create(mat.cols, mat.rows);
    for (size_t i = 0; i < mat.rows; ++i) {
        for (size_t j = 0; j < mat.cols; ++j) {
            matrix_set(&result, j, i, matrix_get(&mat, i, j));
        }
    }
    return result;
}

bool matrix_equal(matrix_t a, matrix_t b, float eps) {
    if (a.rows != b.rows || a.cols != b.cols) return false;
    for (size_t i = 0; i < a.rows; ++i) {
        for (size_t j = 0; j < a.cols; ++j) {
            if (fabsf(matrix_get(&a, i, j) - matrix_get(&b, i, j)) > eps) {
                return false;
            }
        }
    }
    return true;
}

matrix_t matrix_zeros(size_t rows, size_t cols) {
    return matrix_create(rows, cols); // calloc已初始化为0
}

matrix_t matrix_ones(size_t rows, size_t cols) {
    matrix_t mat = matrix_create(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        mat.data[i] = 1.0f;
    }
    return mat;
}

matrix_t matrix_diag(const float* diag, size_t n) {
    matrix_t mat = matrix_create(n, n);
    for (size_t i = 0; i < n; ++i) {
        matrix_set(&mat, i, i, diag[i]);
    }
    return mat;
}

matrix_t matrix_from_array(const float* arr, size_t rows, size_t cols) {
    matrix_t mat = matrix_create(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        mat.data[i] = arr[i];
    }
    return mat;
}

matrix_t matrix_from_2darray(size_t rows, size_t cols, const float arr[rows][cols]) {
    matrix_t mat = matrix_create(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            matrix_set(&mat, i, j, arr[i][j]);
        }
    }
    return mat;
}

vec3_t matrix_mul_vec3(matrix_t mat, vec3_t v) {
    vec3_t result = {0, 0, 0};
    if (mat.rows == 3 && mat.cols == 3) {
        result.x = matrix_get(&mat, 0, 0) * v.x + matrix_get(&mat, 0, 1) * v.y + matrix_get(&mat, 0, 2) * v.z;
        result.y = matrix_get(&mat, 1, 0) * v.x + matrix_get(&mat, 1, 1) * v.y + matrix_get(&mat, 1, 2) * v.z;
        result.z = matrix_get(&mat, 2, 0) * v.x + matrix_get(&mat, 2, 1) * v.y + matrix_get(&mat, 2, 2) * v.z;
    }
    return result;
}

// 构造绕Y轴旋转的4x4矩阵（角度制）
matrix_t matrix_make_oy_rotation(float degrees) {
    float rad = degrees * 3.14159265358979323846f / 180.0f;
    float cos_theta = cosf(rad);
    float sin_theta = sinf(rad);
    float arr[16] = {
        cos_theta, 0, -sin_theta, 0,
        0,         1, 0,          0,
        sin_theta, 0, cos_theta,  0,
        0,         0, 0,          1
    };
    return matrix_from_array(arr, 4, 4);
}

// 构造平移变换的4x4矩阵
matrix_t matrix_make_translation(vec3_t translation) {
    float arr[16] = {
        1, 0, 0, translation.x,
        0, 1, 0, translation.y,
        0, 0, 1, translation.z,
        0, 0, 0, 1
    };
    return matrix_from_array(arr, 4, 4);
}

// 构造缩放变换的4x4矩阵
matrix_t matrix_make_scaling(float scale) {
    float arr[16] = {
        scale, 0,     0,     0,
        0,     scale, 0,     0,
        0,     0,     scale, 0,
        0,     0,     0,     1
    };
    return matrix_from_array(arr, 4, 4);
}

// 4x4矩阵与vec4_t相乘
vec4_t matrix_mul_vec4(matrix_t mat, vec4_t v) {
    vec4_t result = {0, 0, 0, 0};
    if (mat.rows == 4 && mat.cols == 4) {
        result.x = matrix_get(&mat, 0, 0) * v.x + matrix_get(&mat, 0, 1) * v.y + matrix_get(&mat, 0, 2) * v.z + matrix_get(&mat, 0, 3) * v.w;
        result.y = matrix_get(&mat, 1, 0) * v.x + matrix_get(&mat, 1, 1) * v.y + matrix_get(&mat, 1, 2) * v.z + matrix_get(&mat, 1, 3) * v.w;
        result.z = matrix_get(&mat, 2, 0) * v.x + matrix_get(&mat, 2, 1) * v.y + matrix_get(&mat, 2, 2) * v.z + matrix_get(&mat, 2, 3) * v.w;
        result.w = matrix_get(&mat, 3, 0) * v.x + matrix_get(&mat, 3, 1) * v.y + matrix_get(&mat, 3, 2) * v.z + matrix_get(&mat, 3, 3) * v.w;
    }
    return result;
}
