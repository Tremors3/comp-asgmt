
#include <stdlib.h>

/* --------------------------------- NORMAL --------------------------------- */

void adjacent(int N) {
  N = 10;
  int B[] = {}, A[] = {};

  for (int i = 0; i < N; ++i) {
    A[i] = i;
  }

  for (int j = 0; j < N; ++j) {
    B[j] = j;
  }
}

void adjacent_simplify_form(int N) {
  N = 10;
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
  N = 10;
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
  N = 10;
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
  N = 10;
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
  N = 10;
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
  N = 10;
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
  N = 10;
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
  N = 10;
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
  N = 10;
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
  N = 10;
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
  N = 10;
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

void conditional_block_between_exit_and_guard(int N) {
  N = 10;
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

/* ----------------------------- DIFFERENT GUARD ---------------------------- */

void adjacent_differently_guarded_simplify_form(int N) {
  N = 10;
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

/* ------------------------------ SINGLE GUARD ------------------------------ */

void adjacent_global_guard(int N) {
  N = 10;
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  if (N > 0) {
    do {
      A[i] = i;
      i++;
    } while (i < N);

    do {
      B[j] = j;
      j++;
    } while (j < N);
  }
}

void adjacent_nested_guard(int N) {
  N = 10;
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  if (N > 0) {
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
}

void adjacent_one_nested_guard(int N) {
  N = 10;
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  if (N == 0) {
    do {
      A[i] = i;
      i++;
    } while (i < N);
  }
  if (N == 0) {
    if (N == 0) {
      do {
        B[j] = j;
        j++;
      } while (j < N);
    }
  }
}

/* ------------------------------- TRIP COUNT ------------------------------- */

void different_trip_count(int N) {
  N = 10;
  int M = 11;
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  do {
    A[i] = i;
    i++;
  } while (i < N);

  do {
    B[j] = j;
    j++;
  } while (j < M);
}

/* ----------------------------- LOOP DEPENDENCY ---------------------------- */

void dependency_simplify_form(int N) {
  N = 10;
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  do {
    A[i] = i + i;
    i++;
  } while (i < N);

  do {
    B[j] = A[j + 3] + j;
    j++;
  } while (j < N);
}

void dependency(int N) {
  N = 10;
  int B[] = {}, A[] = {};
  int i = 0, j = 0;

  for (i = 0; i < N; i++) {
    A[i] = i + i;
  }

  for (i = 0; i < N; i++) {
    B[i] = A[i + 3] + i;
  }
}

void nested_dependency(int N) {
  N = 10;
  int B[] = {}, A[] = {}, C[] = {}, D[] = {};
  int i = 0, j = 0, l = 0, f = 0;

  do {
    A[i] = i;

    do {
      C[l] = l;
      l++;
    } while (l < N);

    i++;
  } while (i < N);

  do {
    B[j] = j;

    // do {
    //   D[f] = C[f + 3];
    //   f++;
    // } while (f < N);

    j++;
  } while (j < N);
}

/* ----------------------------- IF BODY TESTING ---------------------------- */

void if_body_test(int N) {
  N = 10;
  int B[] = {}, A[] = {}, C[] = {}, D[] = {};
  int i = 0, j = 0, l = 0, f = 0;

  do {
    A[i] = 0;

    if (rand()) {
      A[i] = i;
    } else {
      A[i] = -i;
    }

    i++;
  } while (i < N);

  do {
    A[i] = 0;

    if (rand()) {
      A[i] = i;
    } else {
      A[i] = -i;
    }

    i++;
  } while (i < N);
}


/* --------------------------------- FUSION --------------------------------- */

void fusion_candidate_both_different_body_and_latch(int N) {
  N = 10;
  int B[] = {}, A[] = {};
  int i = 0, j = 0, D = 0;

  do {
    A[i] = i;
    if (N) {
      D = 10;
    }
    i++;
  } while (i < N);

  do {
    B[j] = j;
    if (N) {
      D = 10;
    }
    j++;
  } while (j < N);
}

void fusion_candidate_single_same_body_and_latch(int N) {
  N = 10;
  int B[] = {}, A[] = {};
  int i = 0, j = 0, D = 0;

  do {
    A[i] = i;

    i++;
  } while (i < N);

  do {
    B[j] = j;
    if (N) {
      D = 10;
    }
    j++;
  } while (j < N);
}

void fusion_candidate_both_same_body_and_latch(int N) {
  N = 10;
  int B[] = {}, A[] = {};
  int i = 0, j = 0, D = 0;

  do {
    A[i] = i;

    i++;
  } while (i < N);

  do {
    B[j] = j;
    j++;
  } while (j < N);
}
