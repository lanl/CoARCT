struct C{
  void a1(){}
};

struct D{
  void b1(C &a){ return a.a1(); }
};

namespace N{
struct A{
  void a1(){}
};

struct B{
  void b1(A &a){ return a.a1(); }
};

void g(B &b){
  A a;
  b.b1(a);
  return;
}

} // N::

namespace N2{
struct A{
  void a1(){}
};

struct B{
  void b1(A &a){ return a.a1(); }
};

void g(B &b){
  A a;
  b.b1(a);
  return;
}

} // N2::
