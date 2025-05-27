/* -------------------------------------------------------------------------- */
/* --------------------------------- FUSION --------------------------------- */
/* -------------------------------------------------------------------------- */

#include <stdlib.h>

/* ------------------------------- NOT GUARDED ------------------------------ */

void NotGuarded_NoIf() {
  int N = 10, i = 0, j = 0;
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

void NotGuarded_FirstHasIf() {
  int N = 10, i = 0, j = 0, c = 0;
  int B[N] = {}, A[N] = {};

  do {
    A[i] = i;

    if (rand()) {
      c += 1;
    }

    i++;
  } while (i < N);

  do {
    B[j] = j;

    j++;
  } while (j < N);
}

void NotGuarded_SecondHasIf() {
  int N = 10, i = 0, j = 0, c = 0;
  int B[N] = {}, A[N] = {};

  do {
    A[i] = i;

    i++;
  } while (i < N);

  do {
    B[j] = j;

    if (rand()) {
      c += 1;
    }

    j++;
  } while (j < N);
}

void NotGuarded_BothHaveIf() {
  int N = 10, i = 0, j = 0, c = 0, d = 0, r = rand();
  int B[N] = {}, A[N] = {};

  do {
    A[i] = i;

    if (r) {
      c += 1;
    }

    i++;
  } while (i < N);

  do {
    B[j] = j;

    if (r) {
      d += 1;
    }

    j++;
  } while (j < N);
}

/* --------------------------------- GUARDED -------------------------------- */

void BothGuarded_NoIf() {
  int N = 10, i = 0, j = 0, r = rand();
  int B[N] = {}, A[N] = {};

  if (r) {
    do {
      A[i] = i;
      i++;
    } while (i < N);
  }

  if (r) {
    do {
      B[j] = j;
      j++;
    } while (j < N);
  }
}

void BothGuarded_FirstHasIf() {
  int N = 10, i = 0, j = 0, c = 0, r = rand();
  int B[N] = {}, A[N] = {};

  if (r) {
    do {
      A[i] = i;

      if (r) {
        c += 1;
      }

      i++;
    } while (i < N);
  }

  if (r) {
    do {
      B[j] = j;

      j++;
    } while (j < N);
  }
}

void BothGuarded_SecondHasIf() {
  int N = 10, i = 0, j = 0, c = 0, r = rand();
  int B[N] = {}, A[N] = {};

  if (r) {
    do {
      A[i] = i;

      i++;
    } while (i < N);
  }

  if (r) {
    do {
      B[j] = j;

      if (r) {
        c += 1;
      }

      j++;
    } while (j < N);
  }
}

void BothGuarded_BothHaveIf() {
  int N = 10, i = 0, j = 0, c = 0, d = 0, r = rand();
  int B[N] = {}, A[N] = {};

  if (r) {
    do {
      A[i] = i;

      if (r) {
        c += 1;
      }

      i++;
    } while (i < N);
  }

  if (r) {
    do {
      B[j] = j;

      if (r) {
        d += 1;
      }

      j++;
    } while (j < N);
  }
}

/* ----------------------------------- END ---------------------------------- */
