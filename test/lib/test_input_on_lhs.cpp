// test_input_on_lhs.cc
// T. M. Kelley
// Jan 03, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

/* This is used to test on_lhs() */
struct S
{
  int i;
};

void f(){
  S s = {1};
  s.i = 2;
}

// Here it is as a single line:
// struct S{  int i;};void f(){  S s = {1};  s.i = 2;}

// End of file
