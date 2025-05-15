
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
