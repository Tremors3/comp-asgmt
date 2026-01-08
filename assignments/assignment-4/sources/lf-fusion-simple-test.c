/* -------------------------------------------------------------------------- */
/* --------------------------------- FUSION --------------------------------- */
/* -------------------------------------------------------------------------- */

void NotGuarded() {
  int N = 10, i = 0, j = 0, c = 0;
  int B[N] = {}, A[N] = {};

  do {
    A[i] = i;

    i++;
  } while (i < N);

  do {
    B[j] = j;

    j++;
  } while (j < N);
}

void Guarded() {
  int N = 10, i = 0, j = 0;
  int B[N] = {}, A[N] = {};

  if (N > 0) {
    do {
      A[i] = i;

      i++;
    } while (i < N);
  }

  if (N > 0) {
    do {
      B[j] = j;

      j++;
    } while (j < N);
  }
}

void withIf() {
  int N = 10, i = 0, j = 0;
  int B[N] = {}, A[N] = {};

  do {
    A[i] = i;

    i++;
  } while (i < N);

  do {
    B[j] = j;

    if(N>0) {
      B[j] = j+j;
    }

    j++;
  } while (j < N);
}

void withCounter() {
  int N = 10, i = 0, j = 0, c = 0;
  int B[N] = {}, A[N] = {};

  do {
    A[i] = i;

    i++;
  } while (i < N);

  do {
    B[j] = j;

    c++;
    j++;
  } while (j < N);
}
