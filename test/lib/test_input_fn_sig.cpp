// test_input_fn_sig.cc
// T. M. Kelley
// Mar 02, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

void f(int i, double d){}
void fc(int i, const double d){}
void fc2(int i, double const d){}
void h(int i, double& d){}
void k(int i, double const & d){}

double m(volatile int x, const volatile double y){return double();}
double m2(volatile int x, volatile const double y){return double();}

// End of file
