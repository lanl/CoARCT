// test_input_part_of_assignment.cc
// T. M. Kelley
// Jan 03, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

struct S{ int i = 0;};

void g(int){return;}

void f(){
  S s;
  s.i = 3;
}

//  struct S{ int i = 0;};void g(int){return;}void f(){  S s;  s.i = 3;}
// End of file
