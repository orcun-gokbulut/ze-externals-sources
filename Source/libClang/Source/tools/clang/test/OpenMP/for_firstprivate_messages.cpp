// RUN: %clang_cc1 -verify -fopenmp=libiomp5 %s

void foo() {
}

bool foobool(int argc) {
  return argc;
}

struct S1; // expected-note 2 {{declared here}} expected-note 2 {{forward declaration of 'S1'}}
extern S1 a;
class S2 {
  mutable int a;

public:
  S2() : a(0) {}
  S2(S2 &s2) : a(s2.a) {}
  static float S2s;
  static const float S2sc;
};
const float S2::S2sc = 0;
const S2 b;
const S2 ba[5];
class S3 {
  int a;
  S3 &operator=(const S3 &s3);

public:
  S3() : a(0) {}
  S3(S3 &s3) : a(s3.a) {}
};
const S3 c;
const S3 ca[5];
extern const int f;
class S4 { // expected-note 2 {{'S4' declared here}}
  int a;
  S4();
  S4(const S4 &s4);

public:
  S4(int v) : a(v) {}
};
class S5 { // expected-note 4 {{'S5' declared here}}
  int a;
  S5(const S5 &s5) : a(s5.a) {}

public:
  S5() : a(0) {}
  S5(int v) : a(v) {}
};
class S6 {
  int a;
  S6() : a(0) {}

public:
  S6(const S6 &s6) : a(s6.a) {}
  S6(int v) : a(v) {}
};

S3 h;
#pragma omp threadprivate(h) // expected-note 2 {{defined as threadprivate or thread local}}

template <class I, class C>
int foomain(int argc, char **argv) {
  I e(4); // expected-note {{'e' defined here}}
  C g(5); // expected-note 2 {{'g' defined here}}
  int i;
  int &j = i; // expected-note {{'j' defined here}}
#pragma omp parallel
#pragma omp for firstprivate // expected-error {{expected '(' after 'firstprivate'}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate( // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate() // expected-error {{expected expression}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate(argc // expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate(argc, // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate(argc > 0 ? argv[1] : argv[2]) // expected-error {{expected variable name}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate(argc)
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate(S1) // expected-error {{'S1' does not refer to a value}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate(a, b) // expected-error {{firstprivate variable with incomplete type 'S1'}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate(argv[1]) // expected-error {{expected variable name}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate(e, g) // expected-error 2 {{firstprivate variable must have an accessible, unambiguous copy constructor}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate(h) // expected-error {{threadprivate or thread local variable cannot be firstprivate}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for linear(i) // expected-error {{unexpected OpenMP clause 'linear' in directive '#pragma omp for'}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
  {
    int v = 0;
    int i;                      // expected-note {{variable with automatic storage duration is predetermined as private; perhaps you forget to enclose 'omp for' directive into a parallel or another task region?}}
#pragma omp for firstprivate(i) // expected-error {{private variable cannot be firstprivate}}
    for (int k = 0; k < argc; ++k) {
      i = k;
      v += i;
    }
  }
#pragma omp parallel shared(i)
#pragma omp parallel private(i)
#pragma omp for firstprivate(j) // expected-error {{arguments of OpenMP clause 'firstprivate' cannot be of reference type}}
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for firstprivate(i)
  for (int k = 0; k < argc; ++k)
    ++k;
#pragma omp parallel
#pragma omp for lastprivate(g) firstprivate(g) // expected-error {{firstprivate variable must have an accessible, unambiguous copy constructor}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel private(i) // expected-note {{defined as private}}
#pragma omp for firstprivate(i) // expected-error {{firstprivate variable must be shared}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel reduction(+ : i) // expected-note {{defined as reduction}}
#pragma omp for firstprivate(i)       // expected-error {{firstprivate variable must be shared}}
  for (i = 0; i < argc; ++i)
    foo();
  return 0;
}

int main(int argc, char **argv) {
  const int d = 5;
  const int da[5] = {0};
  S4 e(4); // expected-note {{'e' defined here}}
  S5 g(5); // expected-note 2 {{'g' defined here}}
  S3 m;
  S6 n(2);
  int i;
  int &j = i; // expected-note {{'j' defined here}}
#pragma omp parallel
#pragma omp for firstprivate // expected-error {{expected '(' after 'firstprivate'}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate( // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate() // expected-error {{expected expression}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(argc // expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(argc, // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(argc > 0 ? argv[1] : argv[2]) // expected-error {{expected variable name}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(argc)
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(S1) // expected-error {{'S1' does not refer to a value}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(a, b, c, d, f) // expected-error {{firstprivate variable with incomplete type 'S1'}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(argv[1]) // expected-error {{expected variable name}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(2 * 2) // expected-error {{expected variable name}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(ba) // OK
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(ca) // OK
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(da) // OK
  for (i = 0; i < argc; ++i)
    foo();
  int xa;
#pragma omp parallel
#pragma omp for firstprivate(xa) // OK
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(S2::S2s) // OK
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(S2::S2sc) // OK
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for safelen(5) // expected-error {{unexpected OpenMP clause 'safelen' in directive '#pragma omp for'}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(e, g) // expected-error 2 {{firstprivate variable must have an accessible, unambiguous copy constructor}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(m) // OK
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(h) // expected-error {{threadprivate or thread local variable cannot be firstprivate}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for private(xa), firstprivate(xa) // expected-error {{private variable cannot be firstprivate}} expected-note {{defined as private}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(i) // expected-note {{defined as firstprivate}}
  for (i = 0; i < argc; ++i)    // expected-error {{loop iteration variable in the associated loop of 'omp for' directive may not be firstprivate, predetermined as private}}
    foo();
#pragma omp parallel shared(xa)
#pragma omp for firstprivate(xa) // OK: may be firstprivate
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for firstprivate(j) // expected-error {{arguments of OpenMP clause 'firstprivate' cannot be of reference type}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for lastprivate(g) firstprivate(g) // expected-error {{firstprivate variable must have an accessible, unambiguous copy constructor}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
#pragma omp for lastprivate(n) firstprivate(n) // OK
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel
  {
    int v = 0;
    int i;                      // expected-note {{variable with automatic storage duration is predetermined as private; perhaps you forget to enclose 'omp for' directive into a parallel or another task region?}}
#pragma omp for firstprivate(i) // expected-error {{private variable cannot be firstprivate}}
    for (int k = 0; k < argc; ++k) {
      i = k;
      v += i;
    }
  }
#pragma omp parallel private(i) // expected-note {{defined as private}}
#pragma omp for firstprivate(i) // expected-error {{firstprivate variable must be shared}}
  for (i = 0; i < argc; ++i)
    foo();
#pragma omp parallel reduction(+ : i) // expected-note {{defined as reduction}}
#pragma omp for firstprivate(i)       // expected-error {{firstprivate variable must be shared}}
  for (i = 0; i < argc; ++i)
    foo();

  return foomain<S4, S5>(argc, argv); // expected-note {{in instantiation of function template specialization 'foomain<S4, S5>' requested here}}
}
