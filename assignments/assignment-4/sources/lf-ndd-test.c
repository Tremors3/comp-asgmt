#include <stdlib.h>

/* ------------------------------- ARRAY TESTS ------------------------------ */

/* --------------------- ARRAY - NON NEGATIVE DEPENDANT --------------------- */

void array_NotND_SAME_INDEXES_INCREMENT_OF_1(int N) {
  N = 10;
  int A[N] = {};
  
  int i = 0, j = 0, c = 0;
  do {

    A[i] = i + 1;

    i++;
  } while (i < N);

  do {

    c = A[j];

    j++;
  } while (j < N);
}

void array_NotND_SAME_INDEXES_INCREMENT_OF_2(int N) {
  N = 10;
  int A[N] = {};
  
  int i = 0, j = 0, c = 0;
  do {

    A[i] = i + 1;

    i += 2; // + 2
  } while (i < N);

  do {

    c = A[j];

    j += 2; // + 2
  } while (j < N * 2);
}

void array_NotND_SAME_INDEXES_INCREMENT_OF_3_INLINE(int N) {
  N = 10;
  int A[N] = {};
  
  int i = 0, j = 0, c = 0;
  do {

    A[i + 3] = i + 1; // + 3 inline

    i++;
  } while (i < N);

  do {

    c = A[j + 3]; // + 3 inline

    j++;
  } while (j < N);
}

void array_NotND_SAME_INDEXES_START_AT_4(int N) {
  N = 10;
  int A[N] = {};
  
  int i = 4, j = 4, c = 0; // starts at 4
  do {

    A[i] = i + 1;

    i++;
  } while (i < N + 4);

  do {

    c = A[j];

    j++;
  } while (j < N + 4);
}

/* ----------------------- ARRAY - NEGATIVE DEPENDANT ----------------------- */

void array_IsND_DIFFERENT_INDEXES_INCREMENT_OF_2(int N) {
  N = 10;
  int A[N] = {};
  
  int i = 0, j = 0, c = 0;
  do {

    A[i] = i + 1;

    i++;
  } while (i < N);

  do {

    c = A[j];

    j += 2; // + 2
  } while (j < N);
}

void array_IsND_DIFFERENT_INDEXES_INCREMENT_OF_3_INLINE(int N) {
  N = 10;
  int A[N] = {};
  
  int i = 0, j = 0, c = 0;
  do {

    A[i] = i + 1;

    i++;
  } while (i < N);

  do {

    c = A[j + 3]; // + 3 inline

    j++;
  } while (j < N);
}

void array_IsND_DIFFERENT_INDEXES_START_AT_4(int N) {
  N = 10;
  int A[N] = {};
  
  int i = 0, j = 4, c = 0; // load starts at 4
  do {

    A[i] = i + 1;

    i++;
  } while (i < N);

  do {

    c = A[j];

    j++;
  } while (j < N);
}

/* ------------------------------ MATRIX TESTS ------------------------------ */

/* -------------------- MATRIXES - NON NEGATIVE DEPENDANT ------------------- */

void matrix_NotND_SAME_INDEXES_INCREMENT_OF_1(int N) {
  N = 10;
  int A[N][N] = {}, c = 0;
  
  int i = 0, j = 0;
  int x = 0, y = 0;
  do {

    A[i][j] = i + 1;

    i++;
    j++;
  } while (i < N);

  do {

    c = A[x][y];

    x++;
    y++;
  } while (j < N);
}

/* ---------------------- MATRIXES - NEGATIVE DEPENDANT --------------------- */

void matrix_IsND_DIFFERENT_INDEXES_INCREMENT_OF_1(int N) {
  N = 10;
  int A[N][N] = {}, c = 0;
  
  int i = 0, j = 0;
  int x = 0, y = 0;
  do {

    A[i][j] = i + 1;

    i++;
    j++;
  } while (i < N * 2);

  do {

    c = A[x][y];

    x += 2; // + 2
    y++;
  } while (j < N * 2);
}

void matrix_x4_IsND_DIFFERENT_INDEXES_INCREMENT_OF_2() {
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

/* ----------------------- SPECIAL CASES - ASSUMING ND ---------------------- */

void matrix_IsND_SPECIAL_CASE_RANDOM_INDEX(int N) {
  N = 10;
  int A[N][N] = {}, c = 0;
  
  int i = rand(), j = 0;
  int x = 0, y = 0;
  do {

    A[i][j] = i + 1;

    i += 2;
    j++;
  } while (i < N * 2);

  do {

    c = A[x][y];

    x++;
    y++;
  } while (j < N * 2);
}

void matrix_IsND_SPECIAL_CASE_PARAMETER_INDEX(int N, int i) {
  N = 10;
  int A[N][N] = {}, c = 0;
  
  int j = 0; // i defined as a parameter
  int x = 0, y = 0;
  do {

    A[i][j] = i + 1;

    i += 2;
    j++;
  } while (i < N * 2);

  do {

    c = A[x][y];

    x++;
    y++;
  } while (j < N * 2);
}

/* ----------------------------------- END ---------------------------------- */
