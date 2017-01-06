// test_input_mk_ptr_ref.cc
// T. M. Kelley
// Jan 05, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

// case 1
struct S{
  S * next;
};
void f(S*p){
  while(p != p->next){
    p = p->next;
  }
}

// struct S{  S * next;};void f(S*p){  while(p != p->next){    p = p->next;  }}

// case 2
struct S1{
  S1 * next;
};
void f1(S1*p){
  p = 0;
}
// struct S1{  S1 * next;}; void f1(S1*p){  p = 0;}
// End of file
