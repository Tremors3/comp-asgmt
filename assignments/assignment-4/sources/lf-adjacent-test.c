
#include <stdlib.h>

/* --------------------------------- NORMAL --------------------------------- */

void adjacent(int N) {
  int B[] = {}, A[] = {};

  for (int i = 0; i < N; ++i) {
    A[i] = i;
  }

  for (int j = 0; j < N; ++j) {
    B[j] = j;
  }
}

void adjacent_simplify_form(int N) {
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  do {
    A[i] = i;
    i++;
  } while (i < N);

  do {
    B[j] = j;
    j++;
  } while (j < N);
}

/* -------------------------------- ADJACENT -------------------------------- */

void adjacent_guarded(int N) {
  int B[] = {}, A[] = {};

  if (rand()) {
    for (int i = 0; i < N; ++i) {
      A[i] = i;
    }
  }

  if (rand()) {
    for (int j = 0; j < N; ++j) {
      B[j] = j;
    }
  }
}

void adjacent_guarded_simplify_form(int N) {
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

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

/* ------------------------------- ONE GUARDED ------------------------------ */

void adjacent_one_guarded(int N) {
  int B[] = {}, A[] = {};

  if (rand()) {
    for (int i = 0; i < N; ++i) {
      A[i] = i;
    }
  }

  for (int j = 0; j < N; ++j) {
    B[j] = j;
  }
}

void adjacent_one_guarded_simplify_form(int N) {
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  if (N > 0) {
    do {
      A[i] = i;
      i++;
    } while (i < N);
  }

  do {
    B[j] = j;
    j++;
  } while (j < N);
}

void adjacent_one_guarded_reverse(int N) {
  int B[] = {}, A[] = {};

  for (int i = 0; i < N; ++i) {
    A[i] = i;
  }

  if (rand()) {
    for (int j = 0; j < N; ++j) {
      B[j] = j;
    }
  }
}

void adjacent_one_guarded_simplify_form_reverse(int N) {
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  do {
    A[i] = i;
    i++;
  } while (i < N);

  if (N > 0) {
    do {
      B[j] = j;
      j++;
    } while (j < N);
  }
}

/* ------------------------------ NON ADJACENT ------------------------------ */

void non_adjacent(int N) {
  int B[] = {}, A[] = {};

  for (int i = 0; i < N; ++i) {
    A[i] = i;
  }

  int x = rand();

  for (int j = 0; j < N; ++j) {
    B[j] = j;
  }
}

void non_adjacent_simplify_form(int N) {
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  do {
    A[i] = i;
    i++;
  } while (i < N);

  int x = rand();

  do {
    B[j] = j;
    j++;
  } while (j < N);
}

void non_adjacent_guarded(int N) {
  int B[] = {}, A[] = {};

  if (rand()) {
    for (int i = 0; i < N; ++i) {
      A[i] = i;
    }
  }

  int x = rand();

  if (rand()) {
    for (int j = 0; j < N; ++j) {
      B[j] = j;
    }
  }
}

void non_adjacent_guarded_simplify_form(int N) {
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  if (N > 0) {
    do {
      A[i] = i;
      i++;
    } while (i < N);
  }

  int x = rand();

  if (N > 0) {
    do {
      B[j] = j;
      j++;
    } while (j < N);
  }
}

/* ----------------------------- DIFFERENT GUARD ---------------------------- */

void adjacent_differently_guarded(int N) {
  int B[] = {}, A[] = {};

  if (N > 0) {
    for (int i = 0; i < N; ++i) {
      A[i] = i;
    }
  }

  if (N < 0) {
    for (int j = 0; j < N; ++j) {
      B[j] = j;
    }
  }
}

void adjacent_differently_guarded_simplify_form(int N) {
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  if (N > 0) {
    do {
      A[i] = i;
      i++;
    } while (i < N);
  }

  if (N < 0) {
    do {
      B[j] = j;
      j++;
    } while (j < N);
  }
}
