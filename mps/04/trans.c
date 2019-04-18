/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int block_size;
    int row, column;
    int temp;
    int flag; // indicates if a diagonal element has been used
    int r;

    if (N == 32 && M == 32){// Size of the matrix is 32x32
        block_size = 8; // Blocking with 8 x8 matrix
        for (row = 0; row < N; row += block_size){
            for (column = 0; column < M; column += block_size){
                for (int i = row; i < row + block_size; i++){
                    for (int j = column; j < column +block_size; j++){
                        if (i != j){// Tests if it is a diagnal elemnt
                            B[j][i] = A[i][j];
                        }else{
                            temp = A[i][j];
                            r = i;
                            flag = 1;
                        }
                    }
                    if (flag ==1){
                        B[r][r] = temp;
                        flag = 0;
                    }
                }
            }
        }
    }
    else if (M == 64 && N == 64){
       // block_size = 10; // Blocking with size 4 x 4
       int sub_row = 8;
       int sub_col = 4;
          for (row = 0; row < N; row += sub_row){
                for( column = 0; column < M; column += sub_col){
                  for (int i = row; i < row + sub_row; i++){
                      for (int j = column; j < column +sub_col; j++){
                          if (i != j){// Tests if it is a diagnal elemnt
                              B[j][i] = A[i][j];
                          }else{
                              temp = A[i][j];
                              r = i;
                              flag = 1;
                          }
                      }
                      if (flag ==1){
                          B[r][r] = temp;
                          flag = 0;
                      }
                  }
             }
          }
    
    }
    else if (M == 61 && N == 67){
       int  sub_row = 21;
       int sub_col = 2;
        for (row = 0; row < N; row += sub_row){
            for (column = 0; column < M; column +=sub_col){
                for(int i = row; i < row + sub_row; i++){
                    for (int j = column; j < column + sub_col; j++){
                        if (i > 66 || j > 60){
                            continue;
                        }else{
                            B[j][i]= A[i][j];
                        }
                    }
                }
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

