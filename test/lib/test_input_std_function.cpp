#include <functional>
#include <typeinfo>
#include <iostream>


template <typename T>
void printt(){
  std::cout << typeid(T).name() << "\n";
}

template <typename T>
const char * type_as_string(){return "unknown";}

#define STRINGIFY_TYPE(x) template <> const char * type_as_string<x>(){return #x;}

template <> const char * type_as_string<int>(){return "int";}

STRINGIFY_TYPE(double);

struct Woot{};

class Boot{};

STRINGIFY_TYPE(Woot);

template <typename T>
T adder(T v){
  std::cout << __PRETTY_FUNCTION__ << "\n";
  return v;
}

template <typename T, typename... Ts>
T adder(T first, Ts... args){
  std::cout << __PRETTY_FUNCTION__ << "\n";
  return first + adder(args...);
}

int main(int, char **){
  // printt<double>();
  // printt<int>();
  // printt<uint32_t>();
  // printt<Woot>();
  // printt<Boot>();

  // std::cout << type_as_string<int>() << "\n";
  // std::cout << type_as_string<double>() << "\n";
  // std::cout << type_as_string<Woot>() << "\n";

  std::cout << adder(1,4,6,7) << "\n";
  return 0;
}

