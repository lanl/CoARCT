#include <vector>

template <typename T>
struct A{};

void f(){
  A<int> a;
}

std::vector<int> v(1);

struct B{int b_;};

std::vector<B> vb(1);
