// RUN: %clang_cc1 -fsanitize=memory -fsanitize-memory-use-after-dtor -disable-llvm-optzns -std=c++11 -triple=x86_64-pc-linux -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -O1 -fsanitize=memory -fsanitize-memory-use-after-dtor -disable-llvm-optzns -std=c++11 -triple=x86_64-pc-linux -emit-llvm -o - %s | FileCheck %s

// Only the last dtor of a class invokes the sanitizing callback
// Sanitizing callback emited prior to base class dtor invocations

class Base {
 public:
  int x;
  Base() {
    x = 5;
  }
  virtual ~Base() {
    x += 1;
  }
};

class Derived : public Base {
 public:
  int y;
  Derived() {
    y = 10;
  }
  ~Derived() {
    y += 1;
  }
};

Derived d;

// CHECK-LABEL: define {{.*}}DerivedD1Ev
// CHECK-NOT: call void @__sanitizer_dtor_callback
// CHECK: call void {{.*}}DerivedD2Ev
// CHECK-NOT: call void @__sanitizer_dtor_callback
// CHECK: ret void

// CHECK-LABEL: define {{.*}}DerivedD0Ev
// CHECK-NOT: call void @__sanitizer_dtor_callback
// CHECK: call void {{.*}}DerivedD1Ev
// CHECK-NOT: call void @__sanitizer_dtor_callback
// CHECK: ret void

// CHECK-LABEL: define {{.*}}BaseD1Ev
// CHECK-NOT: call void @__sanitizer_dtor_callback
// CHECK: call void {{.*}}BaseD2Ev
// CHECK-NOT: call void @__sanitizer_dtor_callback
// CHECK: ret void

// CHECK-LABEL: define {{.*}}BaseD0Ev
// CHECK-NOT: call void @__sanitizer_dtor_callback
// CHECK: call void {{.*}}BaseD1Ev
// CHECK-NOT: call void @__sanitizer_dtor_callback
// CHECK: ret void

// CHECK-LABEL: define {{.*}}BaseD2Ev
// CHECK: call void @__sanitizer_dtor_callback
// CHECK-NOT: call void @__sanitizer_dtor_callback
// CHECK: ret void

// CHECK-LABEL: define {{.*}}DerivedD2Ev
// CHECK: call void @__sanitizer_dtor_callback
// CHECK-NOT: call void @__sanitizer_dtor_callback
// CHECK: call void {{.*}}BaseD2Ev
// CHECK-NOT: call void @__sanitizer_dtor_callback
// CHECK: ret void
