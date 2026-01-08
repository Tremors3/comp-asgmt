/* ------------------------- ARRAY, MATRIX NND TESTS ------------------------ */

void array_ndd_test_INDEX_INC(int N) {
  N = 10;
  int A[N] = {};

  int i = 0, j = 0;
  do {

    A[i] = i + 1;

    i++;
  } while (i < N);

  do {

    int c = A[j + 3];

    j++;
  } while (j < N);
}

#include <stdlib.h>

void array_ndd_test_CONSTANT(int N) {
  N = 10;
  int A[N] = {};

  int i = 0, j = 0, d = 9;
  do {
    j++;

    i++;
    A[i + j + d] = i + 1;

    i++;
  } while (i < N);

  do {

    int c = A[j + 3];

    j++;
  } while (j < N);
}

void array_ndd_test_CONSTANT_2(int N) {
  N = 10;
  int A[N] = {};

  int i = 0, j = 0;
  do {
    A[i] = i + 1;

    i++;
  } while (i < N);

  do {
    int c = A[j + 3];

    j++;
  } while (j < N);
}

void matrix_ndd_test() {
  int N = 10;
  int D = N + 3;
  int A[D][D][D][D] = {};

  int i = 0, j = 0;
  do {

    A[i][i][i][i] = i + 1;

    i++;
  } while (i < N);

  do {

    int d = A[j][j][j][j];

    j = j + 2;
  } while (j < N * 2);
}

void matrix_ndd_test_non_so_come_chiamarlo() {
  int N = 10;
  int D = N + 3;
  int A[D + 3][D - 1][D * 4][D / 10] = {};

  int i = 0, j = 0;
  do {
    int ii = 0;
    do {
      int iii = 0;
      do {
        int iiii = 0;
        do {
          A[i][ii][iii][iiii] = i + 1;
          iiii++;
        } while (iiii < N);
        iii++;
      } while (iii < N);
      ii++;
    } while (ii < N);
    i++;
  } while (i < N);

  do {

    int jj = 0;
    do {
      int jjj = 0;
      do {
        int jjjj = 0;
        do {
          int d = A[j][jj][jjj][jjjj];
          jjjj++;
        } while (jjjj < N);
        jjj++;
      } while (jjj < N);
      jj++;
    } while (jj < N);

    j = j + 2;
  } while (j < N * 2);
}


void matrix_ndd_test_multiple_matrices() {
  int a = 0, b = 0;
  int N = 10;
  int D = N + 3;
  int A[D][D][D][D] = {};
  int B[D][D];
  int C[D][D];

  int i = 0, j = 0, k = 0;
  do {

    A[i][i][i][i] = i + 1;
    B[i][i] = i * 2;
    a +=  A[i][i][i][i];

    i++;
  } while (i < N);

  do {

    int d = A[j][j][j+1][j];
    int g = B[k][j];
    C[i][i] = j * 2;

    j++;
    k = k + 2;
  } while (j < N * 2);
}
