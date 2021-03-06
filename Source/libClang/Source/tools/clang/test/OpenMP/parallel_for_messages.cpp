// RUN: %clang_cc1 -verify -fopenmp=libiomp5 -ferror-limit 100 -std=c++11 -o - %s

void foo() {
}

#pragma omp parallel for // expected-error {{unexpected OpenMP directive '#pragma omp parallel for'}}

int main(int argc, char **argv) {
#pragma omp parallel for { // expected-warning {{extra tokens at the end of '#pragma omp parallel for' are ignored}}
  for (int i = 0; i < argc; ++i)
    foo();
#pragma omp parallel for ( // expected-warning {{extra tokens at the end of '#pragma omp parallel for' are ignored}}
  for (int i = 0; i < argc; ++i)
    foo();
#pragma omp parallel for[ // expected-warning {{extra tokens at the end of '#pragma omp parallel for' are ignored}}
  for (int i = 0; i < argc; ++i)
    foo();
#pragma omp parallel for] // expected-warning {{extra tokens at the end of '#pragma omp parallel for' are ignored}}
  for (int i = 0; i < argc; ++i)
    foo();
#pragma omp parallel for) // expected-warning {{extra tokens at the end of '#pragma omp parallel for' are ignored}}
  for (int i = 0; i < argc; ++i)
    foo();
#pragma omp parallel for } // expected-warning {{extra tokens at the end of '#pragma omp parallel for' are ignored}}
  for (int i = 0; i < argc; ++i)
    foo();
#pragma omp parallel for
  for (int i = 0; i < argc; ++i)
    foo();
// expected-warning@+1 {{extra tokens at the end of '#pragma omp parallel for' are ignored}}
#pragma omp parallel for unknown()
  for (int i = 0; i < argc; ++i)
    foo();
L1:
  for (int i = 0; i < argc; ++i)
    foo();
#pragma omp parallel for
  for (int i = 0; i < argc; ++i)
    foo();
#pragma omp parallel for
  for (int i = 0; i < argc; ++i) {
    goto L1; // expected-error {{use of undeclared label 'L1'}}
    argc++;
  }

  for (int i = 0; i < 10; ++i) {
    switch (argc) {
    case (0):
#pragma omp parallel for
      for (int i = 0; i < argc; ++i) {
        foo();
        break; // expected-error {{'break' statement cannot be used in OpenMP for loop}}
        continue;
      }
    default:
      break;
    }
  }
#pragma omp parallel for default(none)
  for (int i = 0; i < 10; ++i)
    ++argc; // expected-error {{variable 'argc' must have explicitly specified data sharing attributes}}

  goto L2; // expected-error {{use of undeclared label 'L2'}}
#pragma omp parallel for
  for (int i = 0; i < argc; ++i)
  L2:
  foo();
#pragma omp parallel for
  for (int i = 0; i < argc; ++i) {
    return 1; // expected-error {{cannot return from OpenMP region}}
  }

  [[]] // expected-error {{an attribute list cannot appear here}}
#pragma omp parallel for
      for (int n = 0; n < 100; ++n) {
  }

  return 0;
}

void test_ordered() {
#pragma omp parallel for ordered ordered // expected-error {{directive '#pragma omp parallel for' cannot contain more than one 'ordered' clause}}
  for (int i = 0; i < 16; ++i)
    ;
}

