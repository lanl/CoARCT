void f(){
  return;
}

void g(){
  f();
  return;
}

/*
void f(){return;} void g(){f(); return;}
*/

void h(){return;}

void i(){return h();}

struct S{
  void h(){return;}

  void i(){g(); return;}
};

void k(S & s){ s.h(); return;}
/*
void f(){return;} void g(){f(); return;} struct S{  void h(){return;}  void i(){g();}};void k(S & s){ s.h(); return;}
*/

void m(S * ps){ ps->h();}
/*
struct S{  void h(){return;}  void i(){return;}};void m(S * ps){ ps->h();}
*/

void n(S s){ s->i();}
/*
struct S{  void h(){return;}  void i(){return;}};void n(S s){ s->i();}
*/

// Example mixing bound methods and unbound functions with same name
/*
void h(){return;}void i(){return h();}struct S{  void h(){return;}  void i(){g(); return;}};void k(S & s){ s.h(); return;}
*/

void p(int i=42);

void q(int foo, double pi = 3.2);

/*
void p(int i=42); void q(int foo, double pi = 3.2);

*/

// Examples for expand_callsite
