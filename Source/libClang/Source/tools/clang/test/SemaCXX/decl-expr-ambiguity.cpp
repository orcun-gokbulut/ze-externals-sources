// RUN: %clang_cc1 -Wno-int-to-pointer-cast -fsyntax-only -verify -pedantic-errors %s
// RUN: %clang_cc1 -Wno-int-to-pointer-cast -fsyntax-only -verify -pedantic-errors -x objective-c++ %s

void f() {
  int a;
  struct S { int m; };
  typedef S *T;

  // Expressions.
  T(a)->m = 7;
  int(a)++; // expected-error {{assignment to cast is illegal}}
  __extension__ int(a)++; // expected-error {{assignment to cast is illegal}}
  __typeof(int)(a,5)<<a; // expected-error {{excess elements in scalar initializer}}
  void(a), ++a;
  if (int(a)+1) {}
  for (int(a)+1;;) {} // expected-warning {{expression result unused}}
  a = sizeof(int()+1);
  a = sizeof(int(1));
  typeof(int()+1) a2; // expected-error {{extension used}}
  (int(1)); // expected-warning {{expression result unused}}

  // type-id
  (int())1; // expected-error {{C-style cast from 'int' to 'int ()' is not allowed}}

  // Declarations.
  int fd(T(a)); // expected-warning {{disambiguated as a function declaration}} expected-note{{add a pair of parentheses}}
  T(*d)(int(p)); // expected-note {{previous}}
  typedef T td(int(p));
  extern T tp(int(p));
  T d3(); // expected-warning {{empty parentheses interpreted as a function declaration}} expected-note {{replace parentheses with an initializer}}
  T d3v(void);
  typedef T d3t();
  extern T f3();
  __typeof(*T()) f4(); // expected-warning {{empty parentheses interpreted as a function declaration}} expected-note {{replace parentheses with an initializer}}
  typedef void *V;
  __typeof(*V()) f5(); // expected-error {{ISO C++ does not allow indirection on operand of type 'V' (aka 'void *')}}
  T multi1,
    multi2(); // expected-warning {{empty parentheses interpreted as a function declaration}} expected-note {{replace parentheses with an initializer}}
  T(d)[5]; // expected-error {{redefinition of 'd'}}
  typeof(int[])(f) = { 1, 2 }; // expected-error {{extension used}}
  void(b)(int);
  int(d2) __attribute__(());
  if (int(a)=1) {}
  int(d3(int()));
}

struct RAII {
  RAII();
  ~RAII();
};

void func();
void func2(short);
namespace N {
  struct S;

  void emptyParens() {
    RAII raii(); // expected-warning {{function declaration}} expected-note {{remove parentheses to declare a variable}}
    int a, b, c, d, e, // expected-note {{change this ',' to a ';' to call 'func'}}
    func(); // expected-warning {{function declaration}} expected-note {{replace parentheses with an initializer}}

    S s(); // expected-warning {{function declaration}}
  }
  void nonEmptyParens() {
    int f = 0, // g = 0; expected-note {{change this ',' to a ';' to call 'func2'}}
    func2(short(f)); // expected-warning {{function declaration}} expected-note {{add a pair of parentheses}}
  }
}

class C { };
void fn(int(C)) { } // void fn(int(*fp)(C c)) { } expected-note{{candidate function}}
                    // not: void fn(int C);
int g(C);

void foo() {
  fn(1); // expected-error {{no matching function}}
  fn(g); // OK
}

namespace PR11874 {
void foo(); // expected-note 3 {{class 'foo' is hidden by a non-type declaration of 'foo' here}}
class foo {};
class bar {
  bar() {
    const foo* f1 = 0; // expected-error {{must use 'class' tag to refer to type 'foo' in this scope}}
    foo* f2 = 0; // expected-error {{must use 'class' tag to refer to type 'foo' in this scope}}
    foo f3; // expected-error {{must use 'class' tag to refer to type 'foo' in this scope}}
  }
};

int baz; // expected-note 2 {{class 'baz' is hidden by a non-type declaration of 'baz' here}}
class baz {};
void fizbin() {
  const baz* b1 = 0; // expected-error {{must use 'class' tag to refer to type 'baz' in this scope}}
  baz* b2; // expected-error {{use of undeclared identifier 'b2'}}
  baz b3; // expected-error {{must use 'class' tag to refer to type 'baz' in this scope}}
}
}
