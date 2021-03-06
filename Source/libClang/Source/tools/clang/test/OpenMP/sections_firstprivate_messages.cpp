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
#pragma omp sections firstprivate // expected-error {{expected '(' after 'firstprivate'}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate( // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate() // expected-error {{expected expression}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(argc // expected-error {{expected ')'}} expected-note {{to match this '('}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(argc, // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(argc > 0 ? argv[1] : argv[2]) // expected-error {{expected variable name}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(argc)
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(S1) // expected-error {{'S1' does not refer to a value}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(a, b) // expected-error {{firstprivate variable with incomplete type 'S1'}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(argv[1]) // expected-error {{expected variable name}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(e, g) // expected-error 2 {{firstprivate variable must have an accessible, unambiguous copy constructor}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(h) // expected-error {{threadprivate or thread local variable cannot be firstprivate}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections linear(i) // expected-error {{unexpected OpenMP clause 'linear' in directive '#pragma omp sections'}}
  {
    foo();
  }
#pragma omp parallel
  {
    int v = 0;
    int i;                           // expected-note {{variable with automatic storage duration is predetermined as private; perhaps you forget to enclose 'omp sections' directive into a parallel or another task region?}}
#pragma omp sections firstprivate(i) // expected-error {{private variable cannot be firstprivate}}
    {
      foo();
    }
    v += i;
  }
#pragma omp parallel shared(i)
#pragma omp parallel private(i)
#pragma omp sections firstprivate(j) // expected-error {{arguments of OpenMP clause 'firstprivate' cannot be of reference type}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(i)
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections lastprivate(g) firstprivate(g) // expected-error {{firstprivate variable must have an accessible, unambiguous copy constructor}}
  {
    foo();
  }
#pragma omp parallel private(i)      // expected-note {{defined as private}}
#pragma omp sections firstprivate(i) // expected-error {{firstprivate variable must be shared}}
  {
    foo();
  }
#pragma omp parallel reduction(+ : i) // expected-note {{defined as reduction}}
#pragma omp sections firstprivate(i)  // expected-error {{firstprivate variable must be shared}}
  {
    foo();
  }
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
#pragma omp sections firstprivate // expected-error {{expected '(' after 'firstprivate'}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate( // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate() // expected-error {{expected expression}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(argc // expected-error {{expected ')'}} expected-note {{to match this '('}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(argc, // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(argc > 0 ? argv[1] : argv[2]) // expected-error {{expected variable name}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(argc)
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(S1) // expected-error {{'S1' does not refer to a value}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(a, b, c, d, f) // expected-error {{firstprivate variable with incomplete type 'S1'}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(argv[1]) // expected-error {{expected variable name}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(2 * 2) // expected-error {{expected variable name}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(ba) // OK
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(ca) // OK
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(da) // OK
  {
    foo();
  }
  int xa;
#pragma omp parallel
#pragma omp sections firstprivate(xa) // OK
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(S2::S2s) // OK
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(S2::S2sc) // OK
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections safelen(5) // expected-error {{unexpected OpenMP clause 'safelen' in directive '#pragma omp sections'}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(e, g) // expected-error 2 {{firstprivate variable must have an accessible, unambiguous copy constructor}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(m) // OK
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(h) // expected-error {{threadprivate or thread local variable cannot be firstprivate}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections private(xa), firstprivate(xa) // expected-error {{private variable cannot be firstprivate}} expected-note {{defined as private}}
  {
    foo();
  }
#pragma omp parallel shared(xa)
#pragma omp sections firstprivate(xa) // OK: may be firstprivate
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections firstprivate(j) // expected-error {{arguments of OpenMP clause 'firstprivate' cannot be of reference type}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections lastprivate(g) firstprivate(g) // expected-error {{firstprivate variable must have an accessible, unambiguous copy constructor}}
  {
    foo();
  }
#pragma omp parallel
#pragma omp sections lastprivate(n) firstprivate(n) // OK
  {
    foo();
  }
#pragma omp parallel
  {
    int v = 0;
    int i;                           // expected-note {{variable with automatic storage duration is predetermined as private; perhaps you forget to enclose 'omp sections' directive into a parallel or another task region?}}
#pragma omp sections firstprivate(i) // expected-error {{private variable cannot be firstprivate}}
    {
      foo();
    }
    v += i;
  }
#pragma omp parallel private(i)      // expected-note {{defined as private}}
#pragma omp sections firstprivate(i) // expected-error {{firstprivate variable must be shared}}
  {
    foo();
  }
#pragma omp parallel reduction(+ : i) // expected-note {{defined as reduction}}
#pragma omp sections firstprivate(i)  // expected-error {{firstprivate variable must be shared}}
  {
    foo();
  }

  return foomain<S4, S5>(argc, argv); // expected-note {{in instantiation of function template specialization 'foomain<S4, S5>' requested here}}
}
