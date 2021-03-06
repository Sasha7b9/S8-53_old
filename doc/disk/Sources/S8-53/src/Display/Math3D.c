// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Math3D.h"
#include <string.h>
#include <math.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FillMatriceRotation(Mat4x4 *mat, float angleX, float angleY, float angleZ)
{
    memset(mat, 0, sizeof(float)* 16);

    Mat4x4 matX = {0};
    Mat4x4 matY = {0};
    Mat4x4 matZ = {0};
    Mat4x4 matTemp = {0};

    float cosX = cos(angleX);
    float sinX = sin(angleX);
    float cosY = cos(angleY);
    float sinY = sin(angleY);
    float cosZ = cos(angleZ);
    float sinZ = sin(angleZ);

    matZ.n[0][0] = cosZ;
    matZ.n[0][1] = sinZ;
    matZ.n[1][0] = -sinZ;
    matZ.n[1][1] = cosZ;
    matZ.n[2][2] = mat->n[3][3] = 1.0f;

    matX.n[1][1] = matX.n[2][2] = cosX;
    matX.n[1][2] = sinX;
    matX.n[2][1] = -sinX;
    matX.n[0][0] = matX.n[3][3] = 1.0f;

    matY.n[0][0] = matY.n[2][2] = cosY;
    matY.n[0][2] = -sinY;
    matY.n[2][0] = sinY;
    matY.n[1][1] = matY.n[3][3] = 1.0f;

    MatrixMul(&matX, &matY, &matTemp);
    MatrixMul(&matTemp, &matZ, mat);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FillMatriceScale(Mat4x4 *mat, float scaleX, float scaleY, float scaleZ)
{
    memset(mat, 0, sizeof(float)* 16);
    mat->n[0][0] = scaleX;
    mat->n[1][1] = scaleY;
    mat->n[2][2] = scaleZ;
    mat->n[3][3] = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void FillMatriceMov(Mat4x4 *mat, float movX, float movY, float movZ)
{
    memset(mat, 0, sizeof(float)* 16);
    mat->n[0][0] = mat->n[1][1] = mat->n[2][2] = mat->n[3][3] = 1.0f;
    mat->n[3][0] = movX;
    mat->n[3][1] = movY;
    mat->n[3][2] = movZ;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void SetMatriceTransform(Mat4x4 *matTransform, Mat4x4 *matScale, Mat4x4 *matRotate, Mat4x4 *matMove)
{
    Mat4x4 matTemp = {0};
    MatrixMul(matScale, matRotate, &matTemp);
    MatrixMul(&matTemp, matMove, matTransform);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void TransformPoint(Mat4x4 *mat, Point *pointIn, Point *pointOut)
{
    pointOut->x = pointIn->x * mat->n[0][0] + pointIn->y * mat->n[1][0] + pointIn->z * mat->n[2][0] + mat->n[3][0];
    pointOut->y = pointIn->x * mat->n[0][1] + pointIn->y * mat->n[1][1] + pointIn->z * mat->n[2][1] + mat->n[3][1];
    pointOut->z = pointIn->z * mat->n[0][2] + pointIn->y * mat->n[1][2] + pointIn->z * mat->n[2][2] + mat->n[3][2];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void PresentPointToScreen(Point *point, float *x, float *y)
{
    *x = point->x * (500.0 / point->z);
    *y = point->y * (500.0 / point->z);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void MatrixMul(Mat4x4 *mat0, Mat4x4 *mat1, Mat4x4 *matOut)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            matOut->n[i][j] =   mat0->n[i][0] * mat1->n[0][j] +
                mat0->n[i][1] * mat1->n[1][j] +
                mat0->n[i][2] * mat1->n[2][j] +
                mat0->n[i][3] * mat1->n[3][j];
        }
    }
}
