void f(){
  return;
}

void g(){
  f();
  return;
}

/*void f(){return;} void g(){f(); return;}*/

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
