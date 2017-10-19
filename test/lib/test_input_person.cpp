class Person{};

class Room{
public:
  void add_person(Person p){}

private:
  Person * ps;
};

template <class T, int N>
class Bag<T,N>{};

int main(){
  Person * p = new Person();
  Bag<Person,42> bagopeeps;
  return 0;
}

