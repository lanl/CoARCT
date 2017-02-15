// test_input_struct_field_user.cc
// Jan 06, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

struct foo_t{
  int i;
};
struct bar_t{
  double k;
};
void f1(foo_t & f){
  f.i = 16;
}
void f2(bar_t & b){
  b.k = 3.14;
}
// struct foo_t{int i;};struct bar_t{double k;};void f1(foo_t & f){f.i = 16;}void f2(bar_t & b){b.k = 3.14;}

// End of file
