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

    j=j+2;
  } while (j < N * 2);
}
